#include "Gun.h"
#include "Scene.h"
#include "TimerSystem.h"
#include "Bullet.h"
#include "Lifetime.h"


dae::CGun::CGun(const GunData& gunData) : m_GunData(gunData)
{
	for (size_t i = 0; i < m_GunData.BulletAmount; i++)
	{
		m_Bullets.push_back(CreateBullet(m_GunData.Shooter, m_GunData.BulletCollisionLayer, m_GunData.BulletCollisionLayerToCollideAgainst));
	}
}

void dae::CGun::Start()
{
	for (size_t i = 0; i < m_Bullets.size(); i++)
	{
		SceneManager::GetInstance().GetCurrentScene().AddGameObjectHandle(m_Bullets[i]);
	}

	m_CurrentTimerSystem = &TimerSystem::GetCurrent();

	Timer timeBetweenShots{ m_GunData.TimeBetweenShots, false };
	m_TimeBetweenShotsTimerKey = m_CurrentTimerSystem->AddTimer(timeBetweenShots);
	
}

void dae::CGun::Shoot()
{
	if (m_GunData.Shooter->IsActive() && m_CurrentTimerSystem->TimerAt(m_TimeBetweenShotsTimerKey).IsFinished())
	{
		m_GunData.Shooter->GetComponent<CStatController>()->OffsetStat(StatType::ShotsFired, 1);

		glm::vec2 where = Owner().Transform().GetWorldTransform().Position;
		m_Bullets[m_CurrentBulletIndex]->GetComponent<CLifeTime>()->Respawn(where);

		m_CurrentTimerSystem->RestartTimer(m_TimeBetweenShotsTimerKey);//time between shots

		m_CurrentBulletIndex = (m_CurrentBulletIndex + 1) % m_Bullets.size();
	}
}

void dae::GunWeaponType::Create(const GameObjectHandle& gameObject)
{
	GunData gunData{};
	gunData.BulletAmount = 10;
	gunData.BulletLifeTime = 1.5;
	gunData.Shooter = gameObject;
	gunData.TimeBetweenShots = 0.3f;
	gunData.BulletCollisionLayer = GalagaCollisionLayers::PlayerBullets;
	gunData.BulletCollisionLayerToCollideAgainst = GalagaCollisionLayers::Enemies;
	CGun gun{ gunData };
	gameObject->AddComponent(gun);
}

void dae::GunWeaponType::Execute(GameObject& obj)
{
	obj.GetComponent<CGun>()->Shoot();
}
