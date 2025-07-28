#include "RoundManager.h"
#include "StatDisplayer.h"
#include <fstream>
#include <iostream>
#include "RoundUI.h"
#include "ParticleSystem.h"
#include "ScoreSaver.h"
#include "Gun.h"
#include "CreateParticleSystem.h"

dae::RoundManager::RoundManager(const std::string& fileName) : 
    m_RoundManagerData(std::make_shared<RoundManagerData>())
{
    if (LoadRoundManagerType(fileName))
    {
        m_RoundManagerData->ParticleSystemGameObj = CreateParticleSystem();
        CreateHighscoreScene();
        CreatePlayers();
        CreateRounds();
    }

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

        m_RoundManagerData->Players.push_back(GalagaPlayer{ currentPlayerPos, 180 , playerType });
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
    std::ifstream file(fileName);

    if (!file)
    {
        std::cout << "\nRoundManager couldn't find file " + fileName;
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

            CEnemyFormation formationOne{ glm::vec2{},    0.3f, data->GetRandomPlayer(), data->GetCurrentRoundType().FirstEnemyFormation};
            formationOne.SubscribeOnFormationDeath(checkFormationDeath);

            CEnemyFormation formationTwo{ glm::vec2{},    0.3f, data->GetRandomPlayer(), data->GetCurrentRoundType().SecondEnemyFormation};
            formationTwo.SubscribeOnFormationDeath(checkFormationDeath);
            formationTwo.SubscribeOnEndFormationSwirlBehaviour([enemyFormationOne]() {

                enemyFormationOne->GetComponent<CEnemyFormation>()->StartEnemySwirlBehaviour(true);
                });
            
            CEnemyFormation formationThree{ glm::vec2{},    0.3f, data->GetRandomPlayer(), data->GetCurrentRoundType().ThirdEnemyFormation};
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

            for (auto& player : data->Players)
            {
                player.AddScene(scene);
            }

        };


    for (size_t i = 0; i < m_RoundManagerData->RoundManagerType.GameRounds.size(); i++)
    {
        m_RoundManagerData->SceneNames.push_back("Round " + std::to_string(i));

        SceneManager::GetInstance().AddScene(m_RoundManagerData->SceneNames.back(), sceneCreationFunction);
    }
}

void dae::RoundManager::CreateHighscoreScene()
{
    auto data = m_RoundManagerData;
    auto highscoreSceneCreation = [data](Scene& scene) {

        scene.AddGameObjectHandle(data->ParticleSystemGameObj);

        float vertOffsetBetweenDisplayers = 60;
        int count{ 0 };

        ScoreSaver scoreSaver{ "Highscore.txt" };
        ScoreData scoreData{};

        for(auto& player : data->Players)
        {
            float xWhere = (g_WindowWidth/ (data->Players.size() + 1)) * (count + 1);

            GameObjectHandle statDisplayerGameObj = scene.CreateGameObject();

            StatDisplayData pointsDisplayData{};
            pointsDisplayData.StatNameTextData.FontData.FontFullPath = "Emulogic-zrEw.ttf";
            pointsDisplayData.StatNameTextData.FontData.FontSize = 13;
            pointsDisplayData.StatNameTextData.Color = { 255, 0, 0 };
            pointsDisplayData.StatNameTextData.Text = "POINTS :";
            pointsDisplayData.StatTypeToDisplay = StatType::Points;
            pointsDisplayData.StatValueColor = { 255, 255, 255 };
            pointsDisplayData.StatValueOffsetFromStatName = { 0,5 };
            pointsDisplayData.Where = { xWhere, (g_WindowHeight/2.f) - vertOffsetBetweenDisplayers };
            pointsDisplayData.FromWho = player.GetGameObjectHandle();

            CStatDisplayer pointsDisplayer{ pointsDisplayData };

            statDisplayerGameObj->AddComponent(pointsDisplayer);

            scoreData.Name = "New Player " + std::to_string(count);
            scoreData.Score = pointsDisplayData.FromWho->GetComponent<CStatController>()->GetStat(StatType::Points, StatCategory::CurrentStat);
            scoreSaver.AddScore(scoreData);
            //------------

            StatDisplayData shotsFiredDisplayData{};
            shotsFiredDisplayData.StatNameTextData.FontData.FontFullPath = "Emulogic-zrEw.ttf";
            shotsFiredDisplayData.StatNameTextData.FontData.FontSize = 13;
            shotsFiredDisplayData.StatNameTextData.Color = { 255, 0, 0 };
            shotsFiredDisplayData.StatNameTextData.Text = "SHOTS FIRED :";
            shotsFiredDisplayData.StatTypeToDisplay = StatType::ShotsFired;
            shotsFiredDisplayData.StatValueColor = { 255, 255, 255 };
            shotsFiredDisplayData.StatValueOffsetFromStatName = { 0,5 };
            shotsFiredDisplayData.Where = { xWhere , (g_WindowHeight / 2.f)};
            shotsFiredDisplayData.FromWho = player.GetGameObjectHandle();

            CStatDisplayer shotsFiredDisplayer{ shotsFiredDisplayData };

            statDisplayerGameObj->AddComponent(shotsFiredDisplayer);

            //------------

            StatDisplayData hitNumDisplayData{};
            hitNumDisplayData.StatNameTextData.FontData.FontFullPath = "Emulogic-zrEw.ttf";
            hitNumDisplayData.StatNameTextData.FontData.FontSize = 13;
            hitNumDisplayData.StatNameTextData.Color = { 255, 0, 0 };
            hitNumDisplayData.StatNameTextData.Text = "HIT NUM :";
            hitNumDisplayData.StatTypeToDisplay = StatType::NumberOfHits;
            hitNumDisplayData.StatValueColor = { 255, 255, 255 };
            hitNumDisplayData.StatValueOffsetFromStatName = { 0,5 };
            hitNumDisplayData.Where = { xWhere, (g_WindowHeight / 2.f) + vertOffsetBetweenDisplayers };
            hitNumDisplayData.FromWho = player.GetGameObjectHandle();

            CStatDisplayer hitNumDisplayer{ hitNumDisplayData };

            statDisplayerGameObj->AddComponent(hitNumDisplayer);

            ++count;
        }


        };

    m_RoundManagerData->HighscoreSceneName = SceneManager::GetInstance().AddScene("HighScore", highscoreSceneCreation).Name();
}
