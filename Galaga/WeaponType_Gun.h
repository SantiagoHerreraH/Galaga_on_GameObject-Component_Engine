#pragma once

#include "WeaponType.h"
#include "CollisionLayers.h"

namespace dae {

	class WeaponType_Gun : public WeaponType {

	public:
		WeaponType_Gun(CollisionLayers bulletCollisionLayer, CollisionLayers  collisionLayerToCollideAgainst) :
			m_BulletCollisionLayer(bulletCollisionLayer),
			m_CollisionLayerToCollideAgainst(collisionLayerToCollideAgainst) {
		}

		void Create(const GameObjectHandle& gameObject) override;
		void Execute(GameObject& obj) override;

	private:
		CollisionLayers m_BulletCollisionLayer{};
		CollisionLayers  m_CollisionLayerToCollideAgainst{};
	};
}