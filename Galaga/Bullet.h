#pragma once

#include "Scene.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Texture.h"
#include "TimerSystem.h"

#include "GalagaStats.h"
#include "CollisionLayers.h"
#include "Lifetime.h"

namespace dae {

	dae::GameObjectHandle CreateBullet(GameObjectHandle owner) {

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

		// -- Lifetime

		dae::CLifeTime lifeTime{ 1.5, true };


		//------ Create GameObject

		GameObjectHandle currentBullet{ std::make_shared<GameObject>()};
		currentBullet->AddComponent(currentTexture);
		currentBullet->AddComponent(rigidBody);
		currentBullet->AddComponent(collider);
		currentBullet->AddComponent(lifeTime);

		currentBullet->SetActive(false);

		return currentBullet;
	}

}