#pragma once

#include "Scene.h"
#include "Text.h"
#include "InputManager.h"

#include "Settings.h"
#include "Component.h"
#include "GalagaStats.h"

namespace dae {

	void CreateCurrentScore(PlayerId playerId, dae::GameObjectHandle& currentScore, dae::GameObjectHandle& currentScoreText)
	{
		currentScore = std::make_shared<GameObject>();

		currentScore->Transform().SetLocalPositionX(g_WindowWidth * 4.3f / 5.f);
		currentScore->Transform().SetLocalPositionY((g_WindowHeight * 1.5f / 5.f) + (50 * playerId));

		CText currentText{ "Emulogic-zrEw.ttf", 13 };
		currentText.SetText("1UP ");
		currentText.SetColor(SDL_Color{ 255,0,0 });

		currentText.SetText("00");
		currentText.SetColor(SDL_Color{ 255,255,255 });
		currentText.Center();
		currentScore->AddComponent(currentText);

		//---------------------

		currentScoreText = std::make_shared<GameObject>();

		float scoreOffsetInY = 5;
		currentScoreText->Transform().SetLocalPositionY(scoreOffsetInY);
		currentText.SetText("1UP ");
		currentText.Center();
		currentScoreText->AddComponent(currentText);
		currentScoreText->Transform().SetParent(*currentScore, ETransformReparentType::KeepLocalTransform);
	}

}