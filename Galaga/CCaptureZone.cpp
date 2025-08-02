#include "CCaptureZone.h"
#include "Scene.h"
#include "MovementActionSequence.h"
#include "Texture.h"
#include "GameTime.h"
#include "Animation.h"
#include "Collider.h"
#include "CollisionLayers.h"
#include "GalagaStats.h"

dae::CCaptureZone::CCaptureZone(GameObjectHandle target, const glm::vec2& relativePos) : 
	m_Target(target),
	m_RelativePos(relativePos),
	m_CapturedTarget(std::make_shared<bool>())
{
}

void dae::CCaptureZone::Start()
{
	m_CaptureZone = CreateCaptureZone(m_Target, SceneManager::GetInstance().GetCurrentScene(), m_RelativePos, m_CapturedTarget);
}

void dae::CCaptureZone::SetActive(bool isActive)
{
	Component::SetActive(isActive);

	if (!isActive && m_CaptureZone && m_PlayerDummy)
	{
		m_CaptureZone->SetActive(false);
		m_PlayerDummy->SetActive(false);
		(*m_CapturedTarget) = false;
	}
}

bool dae::CCaptureZone::CapturedTarget() const
{
	return (*m_CapturedTarget);
}

void dae::CCaptureZone::SetActiveCaptureZone(bool setActive)
{
	m_CaptureZone->SetActive(setActive);
}

dae::GameObjectHandle dae::CCaptureZone::CreateCaptureZone(GameObjectHandle target, dae::Scene& scene, const glm::vec2& relativePos, std::shared_ptr<bool>& outCapturedTarget)

{
	TimerSystem* TIMERSYSTEM{ &TimerSystem::GetFromScene(&scene) };

	//------

	SpriteSheet spriteSheet{ dae::CTextureHandle{"beam.png"}, 1, 3 };
	spriteSheet.TextureHandle().Center();

	CAnimation animation{};
	animation.SetFramesPerSecond(6);
	animation.AddFrame(spriteSheet.GetFrame(0, 0), 1);
	animation.AddFrame(spriteSheet.GetFrame(0, 1), 1);
	animation.AddFrame(spriteSheet.GetFrame(0, 2), 1);

	//------

	Rect rect{};
	rect.Height = (int)spriteSheet.GetScaledCellHeight();
	rect.Width = (int)spriteSheet.GetScaledCellWidth();

	CCollider collider{ rect, (int)CollisionLayers::EnemyBullets };
	collider.AddCollisionTagToCollideWith((int)CollisionLayers::Player);
	collider.CenterRect();

	//------ CREATE GAMEOBJECT

	GameObjectHandle captureZone = scene.CreateGameObject();
	captureZone->Transform().SetLocalPositionX(relativePos.x);
	captureZone->Transform().SetLocalPositionY(relativePos.y);
	captureZone->AddComponent(animation);
	captureZone->AddComponent(collider);

	captureZone->Transform().SetParent(Owner(), ETransformReparentType::KeepLocalTransform);
	captureZone->SetActive(false);


	GameObjectHandle playerDummy = CreatePlayerDummy(scene, glm::vec3{});
	playerDummy->SetActive(false);

	int captureZoneDamage = -1;

	float secondsToWaitBeforeOffsettingStat = 1.f;

	Timer timer{ secondsToWaitBeforeOffsettingStat, true };
	timer.GetOnEndEvent().Subscribe([target, captureZoneDamage, TIMERSYSTEM]() mutable {

		CStatController* statController = target->GetComponent<CStatController>();

		if (statController)
		{
			statController->OffsetStat(StatType::Health, captureZoneDamage);
		}

		});
	TimerKey offsetStatAfterTime_TimerKey = TIMERSYSTEM->AddTimer(timer);

	auto onCaptureZoneCollision = [TIMERSYSTEM, target, playerDummy, offsetStatAfterTime_TimerKey]() mutable {

		target->SetActive(false);//depending on how you code it, this might not work since it will be activated later thanks to the offset stat revival

		playerDummy->Transform().MakeRootNode();
		playerDummy->Transform().SetLocalPosition(target->Transform().GetWorldTransform().Position);
		playerDummy->SetActive(true);

		playerDummy->GetComponent<CMovementActionSequence>()->StartSequence();
		TIMERSYSTEM->RestartTimer(offsetStatAfterTime_TimerKey);

		};

	//So you don't deactivate stuff on collision stay
	TimerKey onCaptureCollisionTimerKey = TIMERSYSTEM->TriggerFunctionAfterSeconds(onCaptureZoneCollision, 0.05f, false, true);

	captureZone->GetComponent<CCollider>()->OnCollisionBeginEvent().Subscribe([TIMERSYSTEM, target, onCaptureCollisionTimerKey, outCapturedTarget](
		const GameObject&,
		const GameObject& other) mutable {

			if (&other == target.get())
			{
				(*outCapturedTarget) = true;
				TIMERSYSTEM->RestartTimer(onCaptureCollisionTimerKey);
			}

		});

	m_PlayerDummy = playerDummy;

	return captureZone;
}

dae::GameObjectHandle dae::CCaptureZone::CreatePlayerDummy(dae::Scene& scene, const glm::vec2& relativePos)
{
	//-----
	CCaptureZone* self = this;

	TransformData textureTransform{};
	textureTransform.Scale = { 0.5f, 0.5f , 0.5f };
	dae::CTextureHandle currentTexture{ "galaga.png" };
	currentTexture.SetTextureTransform(textureTransform);
	currentTexture.Center();

	dae::GameObjectHandle dummy = scene.CreateGameObject();
	dummy->Transform().SetLocalPositionX(relativePos.x);
	dummy->Transform().SetLocalPositionY(relativePos.y);
	dummy->Transform().OverrideWorldScaleWithLocalScale(true);
	dummy->AddComponent(currentTexture);

	CMovementActionSequence actionSequence{ "Player Dummy Movement Sequence" };

	float acceptableRadius = 10;
	float targetDistanceFromParentY = -35;
	float movementDuration = 1.f;
	float rotationSpeed = 360.0f;

	actionSequence.AddAction("Move");
	actionSequence.SetActionDependsOnDuration(false);
	actionSequence.AddActionFunction([dummy, movementDuration, self, targetDistanceFromParentY, acceptableRadius](float, const MovementData&) mutable {

		glm::vec2 goalPos{ self->Owner().Transform().GetWorldTransform().Position };
		goalPos.y += targetDistanceFromParentY;

		glm::vec2 movementPath = goalPos - (glm::vec2)dummy->Transform().GetWorldTransform().Position;

		if (glm::length(movementPath) <= acceptableRadius)
		{
			return true;
		}

		glm::vec2 pos = movementPath * (Time::GetInstance().GetElapsedSeconds() / movementDuration);

		dummy->Transform().MoveLocalPositionX(pos.x);
		dummy->Transform().MoveLocalPositionY(pos.y);

		return false;

		});
	actionSequence.AddActionFunction([dummy, rotationSpeed](float, const MovementData&) mutable {

		float rotationDelta = rotationSpeed * Time::GetInstance().GetElapsedSeconds();

		dummy->Transform().MoveLocalRotationZ(rotationDelta);

		return false;

		});
	actionSequence.AddEndSubAction([self, dummy, targetDistanceFromParentY]() mutable {

		dummy->Transform().SetParent(self->Owner(), ETransformReparentType::KeepWorldTransform);
		dummy->Transform().SetLocalPositionY(targetDistanceFromParentY);
		dummy->Transform().SetLocalRotationZ(180.0f);
		});

	dummy->AddComponent(actionSequence);

	dummy->SetActive(false);

	return dummy;
}