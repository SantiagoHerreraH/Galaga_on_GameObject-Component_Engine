#pragma once

#include "Scene.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Texture.h"
#include "TimerSystem.h"

#include "GalagaStats.h"
#include "GalagaCollisionLayers.h"

namespace dae {

	dae::GameObjectHandle CreateBullets(dae::Scene& scene, GameObjectHandle owner, size_t& outLifeTimerKey) {

		//----- COMPONENTS

		// -- Collider

		Rect rect{};
		rect.Height = 20;
		rect.Width = 10;

		CCollider collider{ rect, int(GalagaCollisionLayers::Bullets)};

		int damage = -1;
		auto damageCollided = [damage, owner](GameObject& self, GameObject& other) mutable
			{
				if (owner.get() != &other)
				{
					CStatController* otherStatController = other.GetComponent<CStatController>();
					CStatController* ownerStatController = owner->GetComponent<CStatController>();

					if (otherStatController)
					{
						otherStatController->OffsetStat(StatType::Health, damage);
						self.SetActive(false);

						if (ownerStatController && otherStatController->HasStat(StatType::Points))
						{
							ownerStatController->OffsetStat(StatType::Points, otherStatController->GetStat(StatType::Points, StatCategory::CurrentStat));
							ownerStatController->OffsetStat(StatType::NumberOfHits, 1);
						}
					}

				}
			};

		
		collider.OnCollisionBeginEvent().Subscribe(damageCollided);

		// -- Rigidbody
		const float bulletSpeed{ 800.f };

		dae::CRigidbody2D rigidBody{};
		rigidBody.SetVelocity((glm::vec2)owner->Transform().GetUpVector() * -bulletSpeed); //because -y is up

		// -- Texture

		dae::CTextureHandle currentTexture{ "bullet.png" };


		//------ Create GameObject

		GameObjectHandle currentBullet{ scene.CreateGameObject() };
		currentBullet->AddComponent(currentTexture);
		currentBullet->AddComponent(rigidBody);
		currentBullet->AddComponent(collider);

		currentBullet->SetActive(false);


		//T----- imed Action For Bullet Lifetime

		auto deactiveAction = [currentBullet]() mutable { currentBullet->SetActive(false); };
		Event<> onTimerEndEvent{};
		onTimerEndEvent.Subscribe(deactiveAction);

		Timer bulletLifetime{ 1.5f, true };
		bulletLifetime.SetOnEndEvent(onTimerEndEvent);
		outLifeTimerKey = TimerSystem::GetFromScene(&scene).AddTimer(bulletLifetime);

		return currentBullet;
	}

}