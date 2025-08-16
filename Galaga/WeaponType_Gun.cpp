#include "WeaponType_Gun.h"
#include "CGun.h"

void dae::WeaponType_Gun::Create(const GameObjectHandle& gameObject)
{
	GunData gunData{};
	gunData.BulletAmount = 10;
	gunData.BulletLifeTime = 1.5;
	gunData.Shooter = gameObject;
	gunData.TimeBetweenShots = 0.3f;
	gunData.BulletCollisionLayer = m_BulletCollisionLayer;
	gunData.BulletCollisionLayerToCollideAgainst = m_CollisionLayerToCollideAgainst;
	gunData.ShootAudio.File = "Sound/ShootSound.wav";
	gunData.ShootAudio.LoopAmount = 0;
	CGun gun{ gunData };
	gameObject->AddComponent(gun);
}

void dae::WeaponType_Gun::Execute(GameObject& obj)
{
	obj.GetComponent<CGun>()->Shoot();
}
