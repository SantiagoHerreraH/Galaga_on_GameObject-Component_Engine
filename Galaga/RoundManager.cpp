#include "RoundManager.h"
#include "StatDisplayer.h"
#include <fstream>
#include <iostream>
#include "RoundUI.h"
#include "ParticleSystem.h"
#include "ScoreSaver.h"
#include "Gun.h"
#include "Misc_CreationFunctions.h"
#include "HighscoreScene.h"
#include "ResourceManager.h"
#include "NameAssignerScene.h"

dae::RoundManager::RoundManager(const std::string& fileName) :
    m_RoundManagerData(std::make_shared<RoundManagerData>()),
    m_FileName(fileName)
{
    if (LoadRoundManagerType(fileName))
    {
        m_RoundManagerData->ParticleSystemGameObj = CreateParticleSystem();
        CreatePlayers();
        CreateNameAssignerScene();
        CreateHighscoreScene();
        CreateRounds();
    }

}

std::string dae::RoundManager::GetNameAssignerSceneName() const
{
    return "CreateNameAssignerScene_" + std::to_string(m_RoundManagerData->Players.size());
}

const std::string& dae::RoundManager::GetFileName() const
{
    return m_FileName;
}

void dae::RoundManager::CreatePlayers()
{
    glm::vec2 currentPlayerPos{};
    currentPlayerPos.y = g_WindowHeight - 50;

    auto data = m_RoundManagerData;

    PlayerType playerType{};
    playerType.PlayerCollisionLayer = CollisionLayers::Player;
    playerType.TextureName = "galaga.png";
    playerType.HasVerticalMovement = true;
    playerType.WeaponType = std::make_shared<GunWeaponType>(CollisionLayers::PlayerBullets, CollisionLayers::Enemies);

    for (size_t i = 0; i < m_RoundManagerData->RoundManagerType.PlayerCount; i++)
    {
        currentPlayerPos.x = int((g_WindowWidth/ (m_RoundManagerData->RoundManagerType.PlayerCount + 1)) * (i + 1) );

        m_RoundManagerData->Players.push_back(Player{ currentPlayerPos, 0 , (int)i, playerType });
    }

   
}

void dae::RoundManager::CreateNameAssignerScene()
{
    NameAssignerSceneData data{};

    for (size_t i = 0; i < m_RoundManagerData->Players.size(); i++)
    {
        data.GameObjectsToAssignName.push_back(m_RoundManagerData->Players[i].GetGameObjectHandle());
    }

    data.SceneName = GetNameAssignerSceneName();
    data.SceneNameUponCompletion = m_FileName + " Round " + std::to_string(1); //first round name

    NameAssignerScene nameAssignerScene{data};
}

bool dae::RoundManager::LoadRoundManagerType(const std::string& fileName)
{
    const auto fullPath = ResourceManager::GetInstance().GetDataPath() / fileName;

    std::ifstream file(fullPath.string());

    if (!file.is_open())
    {
        std::cout << "\nRoundManager couldn't find file " + fullPath.string();
        return false;
    }

    nlohmann::json j{};
    file >> j;
    m_RoundManagerData->RoundManagerType = j.get<RoundManagerType>();

    m_RoundManagerData->RoundManagerType.PlayerCount = std::clamp(m_RoundManagerData->RoundManagerType.PlayerCount, 1, 4);

    return true;
}

void dae::RoundManager::CreateRounds()
{
    auto data = m_RoundManagerData;
    auto sceneCreationFunction = [data](Scene& scene) 
        {
            InputControllerData inputData{};
            inputData.AllowKeyboard = true;
            inputData.HowToTreatKeyboard = data->Players.size() > 1 ? HowToTreatKeyboard::MakeItAnIndependentPlayerId : HowToTreatKeyboard::SharePlayerIdWithFirstController;
            inputData.MaxControllers = data->Players.size() > XUSER_MAX_COUNT ? XUSER_MAX_COUNT : data->Players.size();

            InputManager::GetFromScene(&scene).SetData(inputData);

            CreateHighscore(scene, "HI-SCORE", { g_WindowWidth * 4.3f / 5.f , (g_WindowHeight * 0.8f / 5.f) });

            scene.AddGameObjectHandle(data->ParticleSystemGameObj);

            GameObjectHandle enemyFormationOne{ scene.CreateGameObject() };
            GameObjectHandle enemyFormationTwo{ scene.CreateGameObject() };
            GameObjectHandle enemyFormationThree{ scene.CreateGameObject() };


            std::shared_ptr<int> currentFormationDeaths = std::make_shared<int>(0);
            auto checkFormationDeath = [currentFormationDeaths, data]() mutable
                {
                    ++(*currentFormationDeaths);

                    if ((*currentFormationDeaths) >= 3)//3 == num of formations
                    {
                        (*currentFormationDeaths) = 0;

                        if (data->CanIncreaseRoundNum())
                        {
                            data->IncreaseRoundNum();
                            SceneManager::GetInstance().NextScene();
                        }
                        else
                        {
                            data->ResetRoundNum();
                            SceneManager::GetInstance().ChangeCurrentScene(data->HighscoreSceneName);
                        }
                    }
                };

            CEnemyFormation formationOne{ glm::vec2{g_WindowWidth / 2.f, g_WindowHeight/5.f},    0.3f, 2.5f,  data->GetRandomPlayer(), data->GetCurrentRoundType().FirstEnemyFormation};
            formationOne.SubscribeOnFormationDeath(checkFormationDeath); 
            formationOne.SubscribeOnEndFormationSwirlBehaviour([enemyFormationOne, enemyFormationTwo, enemyFormationThree]() 
                {
                    enemyFormationThree->GetComponent<CEnemyFormation>()->StartSendingTroops();
                    enemyFormationTwo->GetComponent<CEnemyFormation>()->StartSendingTroops();
                    enemyFormationOne->GetComponent<CEnemyFormation>()->StartSendingTroops();
                });

            CEnemyFormation formationTwo{ glm::vec2{g_WindowWidth / 2.f, g_WindowHeight * 2.f / 5.f},    0.3f, 2.5f, data->GetRandomPlayer(), data->GetCurrentRoundType().SecondEnemyFormation};
            formationTwo.SubscribeOnFormationDeath(checkFormationDeath);
            formationTwo.SubscribeOnEndFormationSwirlBehaviour([enemyFormationOne]() {

                enemyFormationOne->GetComponent<CEnemyFormation>()->StartEnemySwirlBehaviour();
                });
            
            CEnemyFormation formationThree{ glm::vec2{g_WindowWidth / 2.f, g_WindowHeight * 3.f / 5.f},    0.3f, 2.5f, data->GetRandomPlayer(), data->GetCurrentRoundType().ThirdEnemyFormation};
            formationThree.SubscribeOnFormationDeath(checkFormationDeath);
            formationThree.SubscribeOnEndFormationSwirlBehaviour([enemyFormationTwo]() {

                enemyFormationTwo->GetComponent<CEnemyFormation>()->StartEnemySwirlBehaviour();
                });


            RoundUI roundUI{ scene, (int)data->CurrentRoundIdx + 1 };
            roundUI.SubscribeOnRoundTextEnd([enemyFormationThree](){
                
                enemyFormationThree->GetComponent<CEnemyFormation>()->StartEnemySwirlBehaviour();
                
                });


            enemyFormationOne->AddComponent(formationOne);
            enemyFormationTwo->AddComponent(formationTwo);
            enemyFormationThree->AddComponent(formationThree);



            glm::vec2 currentPlayerPos{};
            currentPlayerPos.y = g_WindowHeight - 50;

            for (size_t i = 0; i < data->Players.size(); i++)
            {
                currentPlayerPos.x = int((g_WindowWidth / (data->Players.size() + 1.f)) * (i + 1));

                data->Players[i].GetGameObject().Transform().SetLocalPositionX(currentPlayerPos.x);
                data->Players[i].GetGameObject().Transform().SetLocalPositionY(currentPlayerPos.y);
                data->Players[i].GetGameObject().SetActive(true);
                data->Players[i].AddScene(scene);
                data->Players[i].SubscribeOnPlayerDie([data]()
                    {
                        SceneManager::GetInstance().ChangeCurrentScene(data->HighscoreSceneName); 
                        //if one player dies, it is over -> no waiting, more aggressive, you can die on purpose and win -> more tactical juice
                    });
            }

        };


    for (size_t i = 0; i < m_RoundManagerData->RoundManagerType.GameRounds.size(); i++)
    {
        m_RoundManagerData->SceneNames.push_back(m_FileName + " Round " + std::to_string(i + 1));

        SceneManager::GetInstance().AddScene(m_RoundManagerData->SceneNames.back(), sceneCreationFunction);
    }
}

void dae::RoundManager::CreateHighscoreScene()
{
    HighscoreScene highscoreScene{ m_RoundManagerData->Players, "HighScoreScene_" + std::to_string(m_RoundManagerData->Players.size()), m_FileName};
    m_RoundManagerData->HighscoreSceneName = highscoreScene.GetName();
}
