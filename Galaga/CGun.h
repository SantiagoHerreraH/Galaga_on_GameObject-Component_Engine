#pragma once

#include "Component.h"
#include "TimerSystem.h"
#include "CollisionLayers.h"
#include "CAudio.h"

namespace dae {

	struct GunData {

		int BulletAmount{ 10 };
		float BulletLifeTime{};
		CollisionLayers BulletCollisionLayer{};
		CollisionLayers BulletCollisionLayerToCollideAgainst{};
		float TimeBetweenShots{ 0.3f };
		GameObjectHandle Shooter{};
		AudioData ShootAudio{};
	};

	class CGun : public Component
	{
	public:
		CGun(const GunData& gunData);
		void Start()override;
		void Shoot();

	private:
		GunData m_GunData{};
		std::shared_ptr<CAudio> m_ShootAudio{};

		size_t m_CurrentBulletIndex{0};
		TimerSystem* m_CurrentTimerSystem{};
		TimerKey m_TimeBetweenShotsTimerKey{};

		std::vector<GameObjectHandle> m_Bullets{};
	};

}


