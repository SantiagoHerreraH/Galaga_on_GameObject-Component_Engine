#pragma once

#include "Scene.h"
#include "Text.h"

#include "Settings.h"

namespace dae {

	dae::GameObjectHandle CreateCurrentScore(dae::Scene& scene) {

		GameObjectHandle currentScoreText = scene.CreateGameObject();

		currentScoreText->Transform().SetLocalPositionX(g_WindowWidth * 4.3f / 5.f);
		currentScoreText->Transform().SetLocalPositionY(g_WindowHeight * 1.6f / 5.f);

		CText currentText{ "Emulogic-zrEw.ttf", 13 };
		currentText.SetText("1UP ");
		currentText.SetColor(SDL_Color{ 255,0,0 });
		currentText.Center();
		currentScoreText->AddComponent(currentText);

		//---------------------

		GameObjectHandle currentScore = scene.CreateGameObject();

		float scoreOffsetInY = currentText.Data()->GetPixelSize().y + 5;
		currentScore->Transform().SetLocalPositionY(currentScoreText->Transform().GetWorldTransform().Position.y + scoreOffsetInY);


		currentText.SetText("00");
		currentText.SetColor(SDL_Color{ 255,255,255 });
		currentText.Center();
		currentScore->AddComponent(currentText);

		return currentScore;

	}

	std::vector<dae::GameObjectHandle> CreatePlayerHealthIndicator(dae::Scene& scene, dae::PlayerId playerId, int playerHealth, const glm::vec2& startPos, float symbolOffset, float textOffset) {


		//----- PLAYER INDICATOR

		dae::CText currentText{ "Emulogic-zrEw.ttf", 12 };
		currentText.SetText("PLAYER " + std::to_string(playerId + 1));
		currentText.SetColor(SDL_Color{ 255, 0, 0 });
		currentText.Center();

		//-----

		dae::GameObjectHandle healthWords = scene.CreateGameObject();
		healthWords->Transform().SetLocalPositionX(startPos.x);
		healthWords->Transform().SetLocalPositionY(startPos.y);

		healthWords->AddComponent(currentText);

		//----- HEALTH INDICATORS

		std::vector<dae::GameObjectHandle> healthIndicators{};

		//---texture
		dae::TransformData textureTransformData{};
		textureTransformData.Scale = { 0.5f, 0.5f , 0.5f };

		dae::CTextureHandle currentTexture{ "galaga.png" };
		currentTexture.SetTextureTransform(textureTransformData);
		glm::vec2 textureSize = currentTexture.Data()->GetPixelSize();

		textureSize.x *= textureTransformData.Scale.x;
		textureSize.y *= textureTransformData.Scale.y;

		float yOffset = textureSize.y + textOffset;
		float xOffset = startPos.x - (((textureSize.x + symbolOffset) * playerHealth) / 2.f);

		for (size_t i = 0; i < playerHealth; i++)
		{

			dae::GameObjectHandle currentHealthIndicator = scene.CreateGameObject();
			currentHealthIndicator->Transform().SetLocalPositionX(xOffset + ((textureSize.x + symbolOffset) * i));
			currentHealthIndicator->Transform().SetLocalPositionY(startPos.y + yOffset);

			currentHealthIndicator->AddComponent(currentTexture);

			healthIndicators.push_back(currentHealthIndicator);
		}

		return healthIndicators;
	}
}