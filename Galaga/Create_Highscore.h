#pragma once
#include "Scene.h"
#include "CText.h"
#include "ScoreSaver.h"

namespace dae {

	inline void Create_Highscore(dae::Scene& scene, const std::string& titleName, const glm::vec2& pos) {

		TransformData currentTransformData{};
		currentTransformData.Position.x = pos.x;
		currentTransformData.Position.y = pos.y;

		CText currentText{ "Emulogic-zrEw.ttf", 13 };
		currentText.SetText(titleName);
		currentText.SetColor(SDL_Color{ 255,0,0 });
		currentText.Center();


		dae::GameObjectHandle highScoreText{ scene.CreateGameObject() };
		highScoreText->Transform().SetLocalTransform(currentTransformData);
		highScoreText->AddComponent(currentText);

		currentTransformData.Position.y += currentText.Data()->GetPixelSize().y + 5;

		ScoreSaver scoreSaver{};
		ScoreData scoreData = scoreSaver.GetHighscore();

		currentText.SetText(std::to_string(scoreData.Score));
		currentText.SetColor(SDL_Color{ 255,255,255 });
		currentText.Center();

		dae::GameObjectHandle highScore{ scene.CreateGameObject() };
		highScore->Transform().SetLocalTransform(currentTransformData);
		highScore->AddComponent(currentText);

	}

	inline void Create_LastScore(dae::Scene& scene, const std::string& titleName, const std::string& gameModeName, const glm::vec2& pos) {

		TransformData currentTransformData{};
		currentTransformData.Position.x = pos.x;
		currentTransformData.Position.y = pos.y;

		CText currentText{ "Emulogic-zrEw.ttf", 13 };
		currentText.SetText(titleName);
		currentText.SetColor(SDL_Color{ 255,0,0 });
		currentText.Center();


		dae::GameObjectHandle highScoreText{ scene.CreateGameObject() };
		highScoreText->Transform().SetLocalTransform(currentTransformData);
		highScoreText->AddComponent(currentText);

		currentTransformData.Position.y += currentText.Data()->GetPixelSize().y + 5;

		ScoreSaver scoreSaver{};
		int scoreData = scoreSaver.GetLastScore(gameModeName);

		currentText.SetText(std::to_string(scoreData));
		currentText.SetColor(SDL_Color{ 255,255,255 });
		currentText.Center();

		dae::GameObjectHandle highScore{ scene.CreateGameObject() };
		highScore->Transform().SetLocalTransform(currentTransformData);
		highScore->AddComponent(currentText);

	}

	inline void Create_HighscoreInLastScores(dae::Scene& scene, const std::string& titleName, const std::string& gameModeName, int amountOfLastScores, const glm::vec2& pos) {

		TransformData currentTransformData{};
		currentTransformData.Position.x = pos.x;
		currentTransformData.Position.y = pos.y;

		CText currentText{ "Emulogic-zrEw.ttf", 13 };
		currentText.SetText(titleName);
		currentText.SetColor(SDL_Color{ 255,0,0 });
		currentText.Center();


		dae::GameObjectHandle highScoreText{ scene.CreateGameObject() };
		highScoreText->Transform().SetLocalTransform(currentTransformData);
		highScoreText->AddComponent(currentText);

		currentTransformData.Position.y += currentText.Data()->GetPixelSize().y + 5;

		ScoreSaver scoreSaver{};
		int scoreData = scoreSaver.GetHighscoreInTheLastScores(gameModeName, amountOfLastScores).Score;

		currentText.SetText(std::to_string(scoreData));
		currentText.SetColor(SDL_Color{ 255,255,255 });
		currentText.Center();

		dae::GameObjectHandle highScore{ scene.CreateGameObject() };
		highScore->Transform().SetLocalTransform(currentTransformData);
		highScore->AddComponent(currentText);

	}

	
}