#include "Scene_Highscore.h"
#include "CStatDisplayer.h"
#include "ScoreSaver.h"
#include "Create_ParticleSystem.h"
#include "TextCreator.h"
#include "CPlayerController.h"
#include "EventTriggerCommand.h"
#include "Scene_MainMenu.h"
#include "CCollider.h"

dae::Scene_Highscore::Scene_Highscore(std::vector<Player> players, const std::string& sceneName, std::string gameModeName) : m_SceneName(sceneName)
{
    m_OnHighscoreEnd = std::make_shared<Event<>>();
    auto onHighscoreEnd = m_OnHighscoreEnd;

    auto highscoreSceneCreation = [players, gameModeName, onHighscoreEnd](Scene& scene) mutable{

        InputControllerData inputData{};
        inputData.AllowKeyboard = true;
        inputData.HowToTreatKeyboard = players.size() > 1 ? HowToTreatKeyboard::MakeItAnIndependentPlayerId : HowToTreatKeyboard::SharePlayerIdWithFirstController;
        inputData.MaxControllers = players.size() > XUSER_MAX_COUNT ? XUSER_MAX_COUNT : (int)players.size();

        InputManager::GetFromScene(&scene).SetData(inputData);

        auto particleSystemObj = Create_ParticleSystem();
        scene.AddGameObjectHandle(particleSystemObj);

        float vertOffsetBetweenDisplayers = 60;
        int count{ 0 };

        ScoreSaver scoreSaver{};
        ScoreData scoreData{};
        scoreData.GameModeName = gameModeName;

        auto backToMainMenu = [onHighscoreEnd](GameObject&) 
            {
                onHighscoreEnd->Invoke();
                SceneManager::GetInstance().ChangeCurrentScene(Scene_MainMenu::Name()); 
            };

        Event<GameObject&> backToMainMenuEvent{};
        backToMainMenuEvent.Subscribe(backToMainMenu);

        for (auto& player : players)
        {

            scoreData.PlayerName = player.GetGameObject().GetName();
            float xWhere = (g_WindowWidth / float(players.size() + 1)) * (count + 1);


            TextCreator currentText{ scoreData.PlayerName, {xWhere, (g_WindowHeight / 2.f) - (vertOffsetBetweenDisplayers * 2)}, 15, {255, 255, 255} };
            scene.AddGameObjectHandle(currentText.GetGameObjectHandle());

            GameObjectHandle statDisplayerGameObj = scene.CreateGameObject();

            StatDisplayData pointsDisplayData{};
            pointsDisplayData.StatNameTextData.FontData.FontFile = "Emulogic-zrEw.ttf";
            pointsDisplayData.StatNameTextData.FontData.FontSize = 13;
            pointsDisplayData.StatNameTextData.Color = { 255, 0, 0 };
            pointsDisplayData.StatNameTextData.Text = "POINTS :";
            pointsDisplayData.StatTypeToDisplay = GameStatType::Points;
            pointsDisplayData.StatValueColor = { 255, 255, 255 };
            pointsDisplayData.StatValueOffsetFromStatName = { 0, 5 };
            pointsDisplayData.StatValueBaseOffsetMultiplierX = 0;
            pointsDisplayData.StatValueBaseOffsetMultiplierY = 1;
            pointsDisplayData.UpdateAutomatically = false;
            pointsDisplayData.Where = { xWhere, (g_WindowHeight / 2.f) - vertOffsetBetweenDisplayers };
            pointsDisplayData.FromWho = player.GetGameObjectHandle();

            CStatDisplayer pointsDisplayer{ pointsDisplayData };

            statDisplayerGameObj->AddComponent(pointsDisplayer);

            scoreData.Score = pointsDisplayData.FromWho->GetComponent<CGameStatController>()->GetStat(GameStatType::Points, StatCategory::CurrentStat);
            scoreSaver.AddScore(scoreData);

            //------------

            StatDisplayData shotsFiredDisplayData{};
            shotsFiredDisplayData.StatNameTextData.FontData.FontFile = "Emulogic-zrEw.ttf";
            shotsFiredDisplayData.StatNameTextData.FontData.FontSize = 13;
            shotsFiredDisplayData.StatNameTextData.Color = { 255, 0, 0 };
            shotsFiredDisplayData.StatNameTextData.Text = "SHOTS FIRED :";
            shotsFiredDisplayData.StatTypeToDisplay = GameStatType::ShotsFired;
            shotsFiredDisplayData.StatValueColor = { 255, 255, 255 };
            shotsFiredDisplayData.StatValueOffsetFromStatName = { 0,5 };
            shotsFiredDisplayData.StatValueBaseOffsetMultiplierX = 0;
            shotsFiredDisplayData.StatValueBaseOffsetMultiplierY = 1;
            shotsFiredDisplayData.Where = { xWhere , (g_WindowHeight / 2.f) };
            shotsFiredDisplayData.FromWho = player.GetGameObjectHandle();       
            shotsFiredDisplayData.UpdateAutomatically = false;

            CStatDisplayer shotsFiredDisplayer{ shotsFiredDisplayData };

            statDisplayerGameObj->AddComponent(shotsFiredDisplayer);

            //------------

            StatDisplayData hitNumDisplayData{};
            hitNumDisplayData.StatNameTextData.FontData.FontFile = "Emulogic-zrEw.ttf";
            hitNumDisplayData.StatNameTextData.FontData.FontSize = 13;
            hitNumDisplayData.StatNameTextData.Color = { 255, 0, 0 };
            hitNumDisplayData.StatNameTextData.Text = "HIT NUM :";
            hitNumDisplayData.StatTypeToDisplay = GameStatType::NumberOfHits;
            hitNumDisplayData.StatValueColor = { 255, 255, 255 };
            hitNumDisplayData.StatValueOffsetFromStatName = { 0,5 };
            hitNumDisplayData.StatValueBaseOffsetMultiplierX = 0;
            hitNumDisplayData.StatValueBaseOffsetMultiplierY = 1;
            hitNumDisplayData.Where = { xWhere, (g_WindowHeight / 2.f) + vertOffsetBetweenDisplayers };
            hitNumDisplayData.FromWho = player.GetGameObjectHandle();
            hitNumDisplayData.UpdateAutomatically = false;


            CStatDisplayer hitNumDisplayer{ hitNumDisplayData };

            statDisplayerGameObj->AddComponent(hitNumDisplayer);

            int shotsFired =  player.GetGameObject().GetComponent<CGameStatController>()->GetStat(GameStatType::ShotsFired);
            int numberOfHits = player.GetGameObject().GetComponent<CGameStatController>()->GetStat(GameStatType::NumberOfHits);
            float hitMissRatioVal = shotsFired != 0 ? (numberOfHits * 100) / (float)shotsFired : 0;

            TextCreator hitMissRatioText("HIT MISS RATIO: ", { xWhere,   (g_WindowHeight / 2.f) + (vertOffsetBetweenDisplayers * 2) }, 13, SDL_Color{ 255, 0 , 0 });
            TextCreator hitMissRatio("% " + std::to_string(hitMissRatioVal), {xWhere,   (g_WindowHeight / 2.f) + (vertOffsetBetweenDisplayers * 2) + 18}, 13, SDL_Color{255, 255 , 255});

            scene.AddGameObjectHandle(hitMissRatioText.GetGameObjectHandle());
            scene.AddGameObjectHandle(hitMissRatio.GetGameObjectHandle());

            player.GetGameObject().SetActive(true, false);
            player.GetGameObject().GetComponent<CCollider>()->SetActive(false);
            scene.AddGameObjectHandle(player.GetGameObjectHandle());
            CPlayerController& playerController = *player.GetGameObject().GetComponent<CPlayerController>();

            playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_RETURN,	     std::make_shared<EventTriggerCommand>(backToMainMenuEvent) });
            playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_DOWN, GamepadButton::RightShoulder,	 std::make_shared<EventTriggerCommand>(backToMainMenuEvent) });


            ++count;
        }

        TextCreator message("PRESS RETURN OR RIGHT SHOULDER TO EXIT", { int(g_WindowWidth / 2.f),   int(g_WindowHeight * 5.5 / 6.f) }, 13, SDL_Color{ 255, 255 , 255 });
        scene.AddGameObjectHandle(message.GetGameObjectHandle());

        };

    SceneManager::GetInstance().AddScene(m_SceneName, highscoreSceneCreation);

    SubscribeOnHighscoreSceneEnd([players]() mutable
        {
            for (size_t i = 0; i < players.size(); i++)
            {
                players[i].GetGameObject().GetComponent<CGameStatController>()->SetStat(GameStatType::NumberOfHits, 0);
                players[i].GetGameObject().GetComponent<CGameStatController>()->SetStat(GameStatType::Points,       0);
                players[i].GetGameObject().GetComponent<CGameStatController>()->SetStat(GameStatType::ShotsFired,   0);
                players[i].GetGameObject().GetComponent<CGameStatController>()->ResetCurrentStat(GameStatType::Health);
            }

        });
}

void dae::Scene_Highscore::SubscribeOnHighscoreSceneEnd(const std::function<void()>& func)
{
    m_OnHighscoreEnd->Subscribe(func);
}
