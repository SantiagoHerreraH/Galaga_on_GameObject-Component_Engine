#pragma once

#include "Scene.h"
#include "CCollider.h"
#include "CRigidbody2D.h"
#include "CTextureHandle.h"
#include "TimerSystem.h"

#include "CGameStatController.h"
#include "CollisionLayers.h"
#include "CLifetime.h"

namespace dae {

	inline dae::GameObjectHandle Create_Bullet(GameObjectHandle owner, CollisionLayers collisionLayer, CollisionLayers collisionLayerToCollideAgainst) {

		//----- COMPONENTS

		// -- Rigidbody
		const float bulletSpeed{ 800.f };

		dae::CRigidbody2D rigidBody{};
		rigidBody.SetVelocity((glm::vec2)owner->Transform().GetUpVector() * -bulletSpeed); //because -y is up

		// -- Texture

		dae::CTextureHandle currentTexture{ "bullet.png" };

		// -- Collider
		auto bulletSize = currentTexture.GetScaledSize();
		Rect rect{};
		rect.Height = (int)bulletSize.y;
		rect.Width = (int)bulletSize.x;

		CCollider collider{ rect, int(collisionLayer) };
		collider.AddCollisionTagToCollideWith((int)collisionLayerToCollideAgainst);

		int damage = -1;
		auto damageCollided = [damage, owner](GameObject& self, GameObject& other) mutable
			{
				if (owner.get() != &other)
				{
					CGameStatController* otherStatController = other.GetComponent<CGameStatController>();
					CGameStatController* ownerStatController = owner->GetComponent<CGameStatController>();

					if (otherStatController)
					{
						otherStatController->OffsetStat(GameStatType::Health, damage);
						self.SetActive(false);

						if (ownerStatController && otherStatController->HasStat(GameStatType::Points))
						{
							ownerStatController->OffsetStat(GameStatType::Points, otherStatController->GetStat(GameStatType::Points, StatCategory::CurrentStat));
							ownerStatController->OffsetStat(GameStatType::NumberOfHits, 1);
						}
					}

				}
			};


		collider.OnCollisionBeginEvent().Subscribe(damageCollided);

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