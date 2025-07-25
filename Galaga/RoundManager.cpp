#include "RoundManager.h"
#include "StatDisplayer.h"
#include <fstream>
#include <iostream>
#include "RoundUI.h"
#include "ParticleSystem.h"
#include "ScoreSaver.h"

dae::RoundManager::RoundManager(const std::string& fileName) : 
    m_RoundManagerData(std::make_shared<RoundManagerData>())
{
    if (LoadRoundManagerType(fileName))
    {
        CreateParticleSystem();
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

void dae::RoundManager::CreateParticleSystem()
{
    m_RoundManagerData->ParticleSystemGameObj = std::make_shared<GameObject>();

    CParticleSystem particleSystem{};
    
    particleSystem.SetNumber(50);
    particleSystem.SetColorRange({0,0,0}, {255, 255, 255 });
    particleSystem.SetMovementDirectionRange({0,1}, { 0,1 });
    particleSystem.SetPositionRange({0,0}, {g_WindowWidth, g_WindowHeight});
    particleSystem.SetMovementSpeedRange(50, 300);
    particleSystem.SetBounds(true, Rect{0, 0, g_WindowWidth , g_WindowHeight });

    m_RoundManagerData->ParticleSystemGameObj->AddComponent(particleSystem);

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


            data->Player.AddScene(scene);

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

        GameObjectHandle statDisplayerGameObj = scene.CreateGameObject();

        StatDisplayData pointsDisplayData{};
        pointsDisplayData.StatNameTextData.FontData.FontFullPath = "Emulogic-zrEw.ttf";
        pointsDisplayData.StatNameTextData.FontData.FontSize = 13;
        pointsDisplayData.StatNameTextData.Color = { 255, 0, 0 };
        pointsDisplayData.StatNameTextData.Text = "POINTS :";
        pointsDisplayData.StatTypeToDisplay = StatType::Points;
        pointsDisplayData.StatValueColor = { 255, 255, 255 };
        pointsDisplayData.StatValueOffsetFromStatName = { 0,5 };
        pointsDisplayData.Where = { g_WindowWidth / 2.f , (g_WindowHeight/2.f) - vertOffsetBetweenDisplayers };
        pointsDisplayData.FromWho = data->Player.GetGameObjectHandle();

        CStatDisplayer pointsDisplayer{ pointsDisplayData };

        statDisplayerGameObj->AddComponent(pointsDisplayer);


        ScoreSaver scoreSaver{ "Highscore.txt" };
        ScoreData scoreData{};
        scoreData.Name = "New Player";
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
        shotsFiredDisplayData.Where = { g_WindowWidth / 2.f , (g_WindowHeight / 2.f)};
        shotsFiredDisplayData.FromWho = data->Player.GetGameObjectHandle();

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
        hitNumDisplayData.Where = { g_WindowWidth / 2.f , (g_WindowHeight / 2.f) + vertOffsetBetweenDisplayers };
        hitNumDisplayData.FromWho = data->Player.GetGameObjectHandle();

        CStatDisplayer hitNumDisplayer{ hitNumDisplayData };

        statDisplayerGameObj->AddComponent(hitNumDisplayer);
        };

    m_RoundManagerData->HighscoreSceneName = SceneManager::GetInstance().AddScene("HighScore", highscoreSceneCreation).Name();
}
