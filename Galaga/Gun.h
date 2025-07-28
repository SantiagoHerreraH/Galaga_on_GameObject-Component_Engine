#pragma once

#include "Component.h"
#include "TimerSystem.h"
#include "WeaponType.h"
#include "CollisionLayers.h"

namespace dae {

	struct GunData {
		int BulletAmount{ 10 };
		float BulletLifeTime{};
		GalagaCollisionLayers BulletCollisionLayer;
		GalagaCollisionLayers BulletCollisionLayerToCollideAgainst;
		float TimeBetweenShots{ 0.3f };
		GameObjectHandle Shooter;
	};

	class CGun : public Component
	{
	public:
		CGun(const GunData& gunData);
		void Start()override;
		void Shoot();

	private:
		GunData m_GunData;

		size_t m_CurrentBulletIndex{0};
		TimerSystem* m_CurrentTimerSystem;
		TimerKey m_TimeBetweenShotsTimerKey;

		std::vector<GameObjectHandle> m_Bullets;
	};


	class GunWeaponType : public WeaponType {

	public:
		void Create(const GameObjectHandle& gameObject) override;
		void Execute(GameObject& obj) override;

	};

}


