#include "HighscoreScene.h"
#include "StatDisplayer.h"
#include "ScoreSaver.h"
#include "Misc_CreationFunctions.h"
#include "TextCreator.h"
#include "PlayerController.h"
#include "EventTriggerCommand.h"
#include "MainMenu.h"

dae::HighscoreScene::HighscoreScene(std::vector<Player> players, const std::string& sceneName, std::string gameModeName) : m_SceneName(sceneName)
{
    auto highscoreSceneCreation = [players, gameModeName](Scene& scene) mutable{

        auto particleSystemObj = CreateParticleSystem();
        scene.AddGameObjectHandle(particleSystemObj);

        float vertOffsetBetweenDisplayers = 60;
        int count{ 0 };

        ScoreSaver scoreSaver{};
        ScoreData scoreData{};
        scoreData.GameModeName = gameModeName;

        auto backToMainMenu = [](GameObject&) {SceneManager::GetInstance().ChangeCurrentScene(MainMenu::Name()); };
        Event<GameObject&> backToMainMenuEvent{};
        backToMainMenuEvent.Subscribe(backToMainMenu);

        for (auto& player : players)
        {
            float xWhere = (g_WindowWidth / (players.size() + 1)) * (count + 1);

            GameObjectHandle statDisplayerGameObj = scene.CreateGameObject();

            StatDisplayData pointsDisplayData{};
            pointsDisplayData.StatNameTextData.FontData.FontFullPath = "Emulogic-zrEw.ttf";
            pointsDisplayData.StatNameTextData.FontData.FontSize = 13;
            pointsDisplayData.StatNameTextData.Color = { 255, 0, 0 };
            pointsDisplayData.StatNameTextData.Text = "POINTS :";
            pointsDisplayData.StatTypeToDisplay = StatType::Points;
            pointsDisplayData.StatValueColor = { 255, 255, 255 };
            pointsDisplayData.StatValueOffsetFromStatName = { 0, 5 };
            pointsDisplayData.StatValueBaseOffsetMultiplierX = 0;
            pointsDisplayData.StatValueBaseOffsetMultiplierY = 1;
            pointsDisplayData.Where = { xWhere, (g_WindowHeight / 2.f) - vertOffsetBetweenDisplayers };
            pointsDisplayData.FromWho = player.GetGameObjectHandle();

            CStatDisplayer pointsDisplayer{ pointsDisplayData };

            statDisplayerGameObj->AddComponent(pointsDisplayer);

            scoreData.PlayerName = "New Player " + std::to_string(count);
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
            shotsFiredDisplayData.StatValueBaseOffsetMultiplierX = 0;
            shotsFiredDisplayData.StatValueBaseOffsetMultiplierY = 1;
            shotsFiredDisplayData.Where = { xWhere , (g_WindowHeight / 2.f) };
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
            hitNumDisplayData.StatValueBaseOffsetMultiplierX = 0;
            hitNumDisplayData.StatValueBaseOffsetMultiplierY = 1;
            hitNumDisplayData.Where = { xWhere, (g_WindowHeight / 2.f) + vertOffsetBetweenDisplayers };
            hitNumDisplayData.FromWho = player.GetGameObjectHandle();

            CStatDisplayer hitNumDisplayer{ hitNumDisplayData };

            statDisplayerGameObj->AddComponent(hitNumDisplayer);



            scene.AddGameObjectHandle(player.GetGameObjectHandle());
            CPlayerController& playerController = *player.GetGameObject().GetComponent<CPlayerController>();

            playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_SPACE,	     std::make_shared<EventTriggerCommand>(backToMainMenuEvent) });
            playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::ButtonX,	 std::make_shared<EventTriggerCommand>(backToMainMenuEvent) });


            ++count;
        }

        TextCreator message("PRESS SPACE OR X TO EXIT", { int(g_WindowWidth / 2.f),   int(g_WindowHeight * 5.5 / 6.f) }, 13, SDL_Color{ 255, 255 , 255 });
        scene.AddGameObjectHandle(message.GetGameObjectHandle());

        };

    SceneManager::GetInstance().AddScene(m_SceneName, highscoreSceneCreation);
}
