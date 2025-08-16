#pragma once

#include "GameObject.h"
#include "SpriteSheet.h"
#include "CAnimation.h"
#include "CLifetime.h"

namespace dae {

	inline GameObjectHandle Create_Explosion(const std::string& fileName) {

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

		CLifeTime lifetime{ 1.f, true };


		dae::GameObjectHandle explosion{ std::make_shared<GameObject>() };
		explosion->AddComponent(animation);
		explosion->AddComponent(lifetime);

		explosion->SetActive(false);

		return explosion;
	}
}