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

dae::RoundManager::RoundManager(const std::string& fileName) :
    m_RoundManagerData(std::make_shared<RoundManagerData>()),
    m_FileName(fileName)
{
    if (LoadRoundManagerType(fileName))
    {
        m_RoundManagerData->ParticleSystemGameObj = CreateParticleSystem();
        CreatePlayers();
        CreateHighscoreScene();
        CreateRounds();
    }

}

std::string dae::RoundManager::GetFirstRoundName() const
{
    return m_FileName + " Round " + std::to_string(1);
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
    playerType.TextureName = "galaga.png";
    playerType.WeaponType = std::make_shared<GunWeaponType>();

    for (size_t i = 0; i < m_RoundManagerData->RoundManagerType.PlayerCount; i++)
    {
        currentPlayerPos.x = int((g_WindowWidth/ (m_RoundManagerData->RoundManagerType.PlayerCount + 1)) * (i + 1) );

        m_RoundManagerData->Players.push_back(Player{ currentPlayerPos, 0 , (int)i, playerType });
        m_RoundManagerData->Players.back().SubscribeOnPlayerDie([data]()
            {
                ++(data->PlayerDeaths);

                if (data->PlayerDeaths >= data->Players.size())
                {
                    data->PlayerDeaths = 0;
                    SceneManager::GetInstance().ChangeCurrentScene(data->HighscoreSceneName);
                }
            });
    }

   
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
            CreateHighscore(scene, "HI-SCORE", { g_WindowWidth * 4.3f / 5.f , (g_WindowHeight * 0.8f / 5.f) });

            scene.AddGameObjectHandle(data->ParticleSystemGameObj);

            GameObjectHandle enemyFormationOne{ scene.CreateGameObject() };
            GameObjectHandle enemyFormationTwo{ scene.CreateGameObject() };
            GameObjectHandle enemyFormationThree{ scene.CreateGameObject() };


            int currentFormationDeaths = 0;
            auto checkFormationDeath = [currentFormationDeaths, data]() mutable
                {
                    ++currentFormationDeaths;

                    if (currentFormationDeaths >= 3)//3 == num of formations
                    {
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

            for (auto& player : data->Players)
            {
                player.AddScene(scene);
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
    HighscoreScene highscoreScene{ m_RoundManagerData->Players, "HighScoreScene", m_FileName };
    m_RoundManagerData->HighscoreSceneName = highscoreScene.GetName();
}
