#include "RoundManager.h"
#include <fstream>
#include <iostream>
#include "RoundUI.h"

dae::RoundManager::RoundManager(const std::string& fileName) : 
    m_RoundManagerData(std::make_shared<RoundManagerData>())
{
    if (LoadRoundManagerType(fileName))
    {
        CreateHighscoreScene();
        CreatePlayer();
        CreateRounds();
    }

}

void dae::RoundManager::CreatePlayer()
{
    glm::vec2 currentPlayerPos{};
    currentPlayerPos.y = g_WindowHeight - 50;
    currentPlayerPos.x = int(g_WindowWidth / 2.f);

    auto name = m_RoundManagerData->HighscoreSceneName;
    m_RoundManagerData->Player = GalagaPlayer{ currentPlayerPos, 180 };
    m_RoundManagerData->Player.SubscribeOnPlayerDie([name]()
        {
            SceneManager::GetInstance().ChangeCurrentScene(name);
        });
}

bool dae::RoundManager::LoadRoundManagerType(const std::string& fileName)
{
    std::ifstream file(fileName);

    if (!file)
    {
        std::cout << "\nRoundManager couldn't find file " + fileName;
        return false;
    }

    nlohmann::json j{};
    file >> j;
    m_RoundManagerData->RoundManagerType = j.get<RoundManagerType>();

    return true;
}

void dae::RoundManager::CreateRounds()
{
    auto data = m_RoundManagerData;
    auto sceneCreationFunction = [data](Scene& scene) 
        {

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

            CEnemyFormation formationOne{ glm::vec2{},    0.3f, data->Player, data->GetCurrentRoundType().FirstEnemyFormation };
            formationOne.SubscribeOnFormationDeath(checkFormationDeath);

            CEnemyFormation formationTwo{ glm::vec2{},    0.3f, data->Player, data->GetCurrentRoundType().SecondEnemyFormation };
            formationTwo.SubscribeOnFormationDeath(checkFormationDeath);
            formationTwo.SubscribeOnEndFormationSwirlBehaviour([enemyFormationOne]() {

                enemyFormationOne->GetComponent<CEnemyFormation>()->StartEnemySwirlBehaviour(true);
                });
            
            CEnemyFormation formationThree{ glm::vec2{},    0.3f, data->Player, data->GetCurrentRoundType().ThirdEnemyFormation };
            formationThree.SubscribeOnFormationDeath(checkFormationDeath);
            formationThree.SubscribeOnEndFormationSwirlBehaviour([enemyFormationTwo]() {

                enemyFormationTwo->GetComponent<CEnemyFormation>()->StartEnemySwirlBehaviour(false);
                });


            RoundUI roundUI{ scene, data->CurrentRoundIdx };
            roundUI.SubscribeOnRoundTextEnd([enemyFormationThree](){
                
                enemyFormationThree->GetComponent<CEnemyFormation>()->StartEnemySwirlBehaviour(false);
                
                });


            enemyFormationOne->AddComponent(formationOne);
            enemyFormationTwo->AddComponent(formationTwo);
            enemyFormationThree->AddComponent(formationThree);

        };


    for (size_t i = 0; i < m_RoundManagerData->RoundManagerType.GameRounds.size(); i++)
    {
        m_RoundManagerData->SceneNames.push_back("Round " + std::to_string(i));

        SceneManager::GetInstance().AddScene(m_RoundManagerData->SceneNames.back(), sceneCreationFunction);
    }
}

void dae::RoundManager::CreateHighscoreScene()
{
    auto highscoreSceneCreation = [](Scene& scene) {
        
            //FALTA EL HIGHSCORE SCENE CREATION
        };

    m_RoundManagerData->HighscoreSceneName = SceneManager::GetInstance().AddScene("HighScore", highscoreSceneCreation).Name();
}
