#include "CGun.h"
#include "Scene.h"
#include "TimerSystem.h"
#include "Create_Bullet.h"


dae::CGun::CGun(const GunData& gunData) : m_GunData(gunData)
{
	for (size_t i = 0; i < m_GunData.BulletAmount; i++)
	{
		m_Bullets.push_back(Create_Bullet(m_GunData.Shooter, m_GunData.BulletCollisionLayer, m_GunData.BulletCollisionLayerToCollideAgainst));
	}
}

void dae::CGun::Start()
{
	if (!m_GunData.ShootAudio.File.empty())
	{
		m_ShootAudio = Owner().AddComponent(CAudio{ m_GunData.ShootAudio });
	}

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
	if (m_CurrentTimerSystem &&
		m_CurrentTimerSystem->HasTimer(m_TimeBetweenShotsTimerKey) &&
		m_GunData.Shooter->IsActive() && 
		m_CurrentTimerSystem->TimerAt(m_TimeBetweenShotsTimerKey).IsFinished())
	{
		m_GunData.Shooter->GetComponent<CGameStatController>()->OffsetStat(GameStatType::ShotsFired, 1);
		m_ShootAudio->Play();

		glm::vec2 where = Owner().Transform().GetWorldTransform().Position;
		m_Bullets[m_CurrentBulletIndex]->GetComponent<CLifeTime>()->Respawn(where);

		m_CurrentTimerSystem->RestartTimer(m_TimeBetweenShotsTimerKey);//time between shots

		m_CurrentBulletIndex = (m_CurrentBulletIndex + 1) % m_Bullets.size();
	}
}


