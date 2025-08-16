#include "RoundManager.h"

#include <fstream>
#include <iostream>

#include "CStatDisplayer.h"
#include "RoundUI.h"
#include "ScoreSaver.h"
#include "CGun.h"
#include "Create_ParticleSystem.h"
#include "Create_Highscore.h"
#include "Scene_Highscore.h"
#include "Scene_NameAssigner.h"
#include "ResourceManager.h"
#include "CPlayerController.h"
#include "EventTriggerCommand.h"
#include "ServiceLocator.h"
#include "WeaponType_Gun.h"

dae::RoundManager::RoundManager(const std::string& fileName) :
    m_RoundManagerData(std::make_shared<RoundManagerData>(fileName)),
    m_FileName(fileName)
{
    if (LoadRoundManagerType(fileName))
    {
        m_RoundManagerData->ParticleSystemGameObj = Create_ParticleSystem();
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
    playerType.TextureScale = 0.5f;
    playerType.HasVerticalMovement = true;
    playerType.WeaponType = std::make_shared<WeaponType_Gun>(CollisionLayers::PlayerBullets, CollisionLayers::Enemies);

    auto nextScene = [data](GameObject&) mutable
        {
            if (SceneManager::GetInstance().GetCurrentScene().Name() == data->HighscoreSceneName)
            {
                data->ResetRoundNum();
                SceneManager::GetInstance().ChangeCurrentScene(data->GetRoundName(data->CurrentRoundIdx));

            }
            else if (data->CanIncreaseRoundNum())
            {
                data->IncreaseRoundNum();
                SceneManager::GetInstance().ChangeCurrentScene(data->GetRoundName(data->CurrentRoundIdx));
            }
            else if(SceneManager::GetInstance().GetCurrentScene().Name() != data->HighscoreSceneName)
            {
                data->ResetRoundNum();
                SceneManager::GetInstance().ChangeCurrentScene(data->HighscoreSceneName);
            }
            
        };


    Event<GameObject&> nextSceneEvent{};
    nextSceneEvent.Subscribe(nextScene);

    for (size_t i = 0; i < m_RoundManagerData->RoundManagerType.PlayerCount; i++)
    {
        currentPlayerPos.x = (g_WindowWidth/ float(m_RoundManagerData->RoundManagerType.PlayerCount + 1)) * (i + 1) ;

        m_RoundManagerData->Players.emplace_back(Player{ currentPlayerPos, 0 , (int)i, playerType });
        auto playerController = m_RoundManagerData->Players.back().GetGameObject().GetComponent<CPlayerController>();
        
        playerController->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_F1,	         std::make_shared<EventTriggerCommand>(nextSceneEvent) });
        playerController->BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_DOWN, GamepadButton::Start,	     std::make_shared<EventTriggerCommand>(nextSceneEvent) });
        

    }

   
}

void dae::RoundManager::CreateNameAssignerScene()
{
    SceneData_NameAssigner data{};

    for (size_t i = 0; i < m_RoundManagerData->Players.size(); i++)
    {
        data.GameObjectsToAssignName.push_back(m_RoundManagerData->Players[i].GetGameObjectHandle());
    }

    data.SceneName = GetNameAssignerSceneName();
    data.SceneNameUponCompletion = m_RoundManagerData->GetRoundName(0); //first round name

    Scene_NameAssigner nameAssignerScene{data};
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
            inputData.MaxControllers = data->Players.size() > XUSER_MAX_COUNT ? XUSER_MAX_COUNT : (int)data->Players.size();

            InputManager::GetFromScene(&scene).SetData(inputData);

            Create_Highscore(scene, "HI-SCORE", { g_WindowWidth * 4.3f / 5.f , (g_WindowHeight * 0.8f / 5.f) });

            scene.AddGameObjectHandle(data->ParticleSystemGameObj);

            GameObjectHandle enemyFormationOne{ scene.CreateGameObject() };
            GameObjectHandle enemyFormationTwo{ scene.CreateGameObject() };
            GameObjectHandle enemyFormationThree{ scene.CreateGameObject() };

            const float xOffsetForFormation = 30;

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

            CEnemyFormation formationOne{ glm::vec2{(g_WindowWidth / 2.f) - xOffsetForFormation, g_WindowHeight/5.f},    0.3f, 2.5f,  data->GetRandomPlayer(), data->GetCurrentRoundType().FirstEnemyFormation};
            formationOne.SubscribeOnFormationDeath(checkFormationDeath); 
            formationOne.SubscribeOnEndFormationSwirlBehaviour([enemyFormationOne, enemyFormationTwo, enemyFormationThree]() 
                {
                    enemyFormationThree->GetComponent<CEnemyFormation>()->StartSendingTroops();
                    enemyFormationTwo->GetComponent<CEnemyFormation>()->StartSendingTroops();
                    enemyFormationOne->GetComponent<CEnemyFormation>()->StartSendingTroops();
                });

            CEnemyFormation formationTwo{ glm::vec2{(g_WindowWidth / 2.f) - xOffsetForFormation, g_WindowHeight * 2.f / 5.f},    0.3f, 2.5f, data->GetRandomPlayer(), data->GetCurrentRoundType().SecondEnemyFormation};
            formationTwo.SubscribeOnFormationDeath(checkFormationDeath);
            formationTwo.SubscribeOnEndFormationSwirlBehaviour([enemyFormationOne]() {

                enemyFormationOne->GetComponent<CEnemyFormation>()->StartEnemySwirlBehaviour();
                });
            
            CEnemyFormation formationThree{ glm::vec2{(g_WindowWidth / 2.f) - xOffsetForFormation, g_WindowHeight * 3.f / 5.f},    0.3f, 2.5f, data->GetRandomPlayer(), data->GetCurrentRoundType().ThirdEnemyFormation};
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
                currentPlayerPos.x = (g_WindowWidth / float(data->Players.size() + 1.f)) * (i + 1);

                data->Players[i].GetGameObject().Transform().SetLocalPositionX(currentPlayerPos.x);
                data->Players[i].GetGameObject().Transform().SetLocalPositionY(currentPlayerPos.y);
                data->Players[i].GetGameObject().SetActive(true);
                data->Players[i].AddScene(scene);
                data->Players[i].SubscribeOnPlayerDie([data]()
                    {
                        data->ResetRoundNum();
                        SceneManager::GetInstance().ChangeCurrentScene(data->HighscoreSceneName); 
                        //if one player dies, it is over -> no waiting, more aggressive, you can die on purpose and win -> more tactical juice
                    });
            }

        };


    for (size_t i = 0; i < m_RoundManagerData->RoundManagerType.GameRounds.size(); i++)
    {
        m_RoundManagerData->SceneNames.push_back(m_RoundManagerData->GetRoundName(i));

        SceneManager::GetInstance().AddScene(m_RoundManagerData->SceneNames.back(), sceneCreationFunction);
    }
}

void dae::RoundManager::CreateHighscoreScene()
{
    Scene_Highscore highscoreScene{ m_RoundManagerData->Players, "HighScoreScene_" + std::to_string(m_RoundManagerData->Players.size()), m_FileName};
    m_RoundManagerData->HighscoreSceneName = highscoreScene.GetName();
}
