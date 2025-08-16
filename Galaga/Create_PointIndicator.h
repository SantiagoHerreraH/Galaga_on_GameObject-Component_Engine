#pragma once

#include "Scene.h"
#include "CText.h"

namespace dae {

	GameObjectHandle Create_PointIndicator(Scene& scene, int value) {
		TransformData currentTransformData{};

		CText currentText{ "Emulogic-zrEw.ttf", 8 };
		currentText.SetText(std::to_string(value));
		currentText.SetColor(SDL_Color{ 255,0,0 });
		currentText.Center();

		dae::GameObjectHandle highScoreText{ scene.CreateGameObject() }; 
	
		highScoreText->AddComponent(currentText);
		highScoreText->SetActive(false);

		return highScoreText;
	}
}