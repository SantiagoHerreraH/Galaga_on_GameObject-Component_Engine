#pragma once
#include "ParticleSystem.h"
#include "Scene.h"
#include "Settings.h"
#include "Text.h"
#include "Lifetime.h"
#include "Animation.h"
#include "ScoreSaver.h"

namespace dae {

    inline GameObjectHandle CreateParticleSystem() {
        GameObjectHandle particleSystemObj = std::make_shared<GameObject>();

        CParticleSystem particleSystem{};

        particleSystem.SetNumber(50);
        particleSystem.SetColorRange({ 0,0,0 }, { 255, 255, 255 });
        particleSystem.SetMovementDirectionRange({ 0,1 }, { 0,1 });
        particleSystem.SetPositionRange({ 0,0 }, { g_WindowWidth, g_WindowHeight });
        particleSystem.SetMovementSpeedRange(50, 300);
        particleSystem.SetBounds(true, Rect{ 0, 0, g_WindowWidth , g_WindowHeight });

        particleSystemObj->AddComponent(particleSystem);

        return particleSystemObj;
	}

	inline void CreateHighscore(dae::Scene& scene, const std::string& titleName, const glm::vec2& pos) {

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

	inline void CreateLastScore(dae::Scene& scene, const std::string& titleName, const std::string& gameModeName, const glm::vec2& pos) {

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

	inline void CreateHighscoreInLastScores(dae::Scene& scene, const std::string& titleName, const std::string& gameModeName, int amountOfLastScores, const glm::vec2& pos) {

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

	inline GameObjectHandle CreateExplosion(const std::string& fileName) {

		SpriteSheet spritesheet{ dae::CTextureHandle{fileName}, 1, 4 };

		CAnimation animation{};
		animation.SetFramesPerSecond(12);
		animation.AddFrame(spritesheet.GetFrame(0, 0), 1);
		animation.AddFrame(spritesheet.GetFrame(0, 1), 1);
		animation.AddFrame(spritesheet.GetFrame(0, 2), 2);
		animation.AddFrame(spritesheet.GetFrame(0, 3), 4);
		animation.AddFrame(spritesheet.GetFrame(0, 2), 2);
		animation.AddFrame(spritesheet.GetFrame(0, 1), 1);
		animation.AddFrame(spritesheet.GetFrame(0, 0), 1);

		CLifeTime lifetime{1.f, true};
		

		dae::GameObjectHandle explosion{ std::make_shared<GameObject>()};
		explosion->AddComponent(animation);
		explosion->AddComponent(lifetime);

		explosion->SetActive(false);

		return explosion;
	}
}