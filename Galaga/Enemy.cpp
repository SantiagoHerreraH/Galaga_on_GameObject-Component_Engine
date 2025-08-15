#include "Enemy.h"
#include "Scene.h"
#include "MovementActionSequence.h"
#include "Animation.h"
#include "Collider.h"
#include "CollisionLayers.h"
#include "GalagaStats.h"
#include "EnemyPointsIndicator.h"
#include "Random.hpp"
#include "Behaviour_CaptureZone.h"
#include "Behaviour_UDive.h"
#include "Behaviour_ZigZagDive.h"
#include "EnemyFormation.h"
#include "Misc_CreationFunctions.h"
#include "Audio.h"

dae::Enemy::Enemy(const EnemyInstanceData& enemyInstanceData, const EnemyType& enemyType) :
	m_EnemyInstanceData(enemyInstanceData),
	m_EnemyType(enemyType)
{
	m_OnEndStartingFormationSequence = std::make_shared<Event<GameObject&>>();
	GameObjectHandle player{ enemyInstanceData.Player };

	TransformData transformData{};
	transformData.Position.x = enemyInstanceData.RelativePos.x;
	transformData.Position.y = enemyInstanceData.RelativePos.y;
	transformData.Scale = { 1, 1, 1 };
	//transformData.Rotation.z = 180;

	//------

	SpriteSheet spriteSheet{ dae::CTextureHandle{enemyType.TextureName}, 1, 2 };

	CAnimation animation{};
	animation.SetFramesPerSecond(2);
	animation.AddFrame(spriteSheet.GetFrame(0, 0), 2);
	animation.AddFrame(spriteSheet.GetFrame(0, 1), 1);


	SpriteSheet secondStageSpriteSheet{ dae::CTextureHandle{"boss2.png"}, 1, 2};

	CAnimation secondStageAnimation{};
	secondStageAnimation.SetFramesPerSecond(2);
	secondStageAnimation.AddFrame(secondStageSpriteSheet.GetFrame(0, 0), 2);
	secondStageAnimation.AddFrame(secondStageSpriteSheet.GetFrame(0, 1), 1);

	//------

	Rect rect{};
	rect.Height = 32;
	rect.Width = 32;

	CCollider collider{ rect, (int)CollisionLayers::Enemies };
	collider.AddCollisionTagToCollideWith((int)CollisionLayers::Player);
	collider.CenterRect();

	collider.OnCollisionBeginEvent().Subscribe([](GameObject&, GameObject& other) mutable
		{
			other.GetComponent<CStatController>()->OffsetStat(StatType::Health, -1);

		});

	//------

	CStatController statController{};

	IntStat initialHealth{ enemyType.MaxHealth, enemyType.MaxHealth, enemyType.MaxHealth };
	IntStat initialKills{ 0, 0, 0 };

	statController.CreateStat(StatType::Health, initialHealth);


	//------

	AudioData dieAudioData{};
	dieAudioData.File = "Sound/EnemyDeathSound.wav";
	dieAudioData.LoopAmount = 0;


	AudioData diveAudioData{};
	diveAudioData.File = "Sound/DiveSound.wav";
	diveAudioData.LoopAmount = 0;


	//------ CREATE GAMEOBJECT

	GameObjectHandle enemy{};

	if (enemyInstanceData.Enemy == nullptr)
	{
		enemy = enemyInstanceData.Scene->CreateGameObject();
	}
	else
	{
		enemy = enemyInstanceData.Enemy;
	}

	enemy->SetName("Enemy");

	m_Self = enemy;
	enemy->Transform().SetLocalTransform(transformData);
	enemy->AddComponent(animation);
	enemy->AddComponent(collider);
	enemy->AddComponent(statController);
	auto dieAudio = enemy->AddComponent(CAudio{ dieAudioData });
	m_OnActAudio = enemy->AddComponent(CAudio{ diveAudioData });


	enemy->Transform().SetParent(*enemyInstanceData.Grid, ETransformReparentType::KeepLocalTransform);
	enemy->Transform().OverrideWorldScaleWithLocalScale(true);

	

	if (enemyType.MaxHealth > 1)
	{
		enemy->GetComponent<CStatController>()->OnCurrentStatChange(StatType::Health).Subscribe([enemy, secondStageAnimation](float value)
			{
				enemy->SetComponent<CAnimation>(secondStageAnimation);

			});
	}

	//-------- POINTS

	const float showPointsDuration{ 1.f };

	//-- POINTS IN FORMATION

	TimerSystem* TIMERSYSTEM = &TimerSystem::GetFromScene(enemyInstanceData.Scene);

	IntStat pointsInFormation{ enemyType.PointsInFormation, enemyType.PointsInFormation, enemyType.PointsInFormation };
	GameObjectHandle pointsInFormationIndicator{ CreatePointIndicator(*enemyInstanceData.Scene, pointsInFormation.CurrentStat) };

	TimerKey showPointsInFormationKey = TIMERSYSTEM->TriggerFunctionAfterSeconds(
		[pointsInFormationIndicator]() mutable {pointsInFormationIndicator->SetActive(false); },
		showPointsDuration,
		false,
		true);

	pointsInFormation.OnResetCurrentStat.Subscribe([TIMERSYSTEM, pointsInFormationIndicator, enemy, showPointsInFormationKey]() {

		pointsInFormationIndicator->Transform().SetLocalPosition(enemy->Transform().GetWorldTransform().Position);
		pointsInFormationIndicator->SetActive(true);
		TIMERSYSTEM->RestartTimer(showPointsInFormationKey);

		});

	//-- POINTS WHILE DIVING

	IntStat pointsWhileDiving{ enemyType.PointOnDive, enemyType.PointOnDive, enemyType.PointOnDive };
	GameObjectHandle pointsWhileDivingIndicator{ CreatePointIndicator(*enemyInstanceData.Scene, pointsWhileDiving.CurrentStat) };
	TimerKey showPointsWhileDivingKey = TIMERSYSTEM->TriggerFunctionAfterSeconds(
		[pointsWhileDivingIndicator]() mutable { pointsWhileDivingIndicator->SetActive(false); },
		showPointsDuration, 
		false,
		true);

	pointsWhileDiving.OnResetCurrentStat.Subscribe([TIMERSYSTEM, showPointsWhileDivingKey, pointsWhileDivingIndicator, enemy]() {

		pointsWhileDivingIndicator->Transform().SetLocalPosition(enemy->Transform().GetWorldTransform().Position);
		pointsWhileDivingIndicator->SetActive(true);
		TIMERSYSTEM->RestartTimer(showPointsWhileDivingKey);

		});

	enemy->GetComponent<CStatController>()->CreateStat(StatType::Points, pointsInFormation);

	m_PointsWhileDiving = pointsWhileDiving;
	m_PointsInFormation = pointsInFormation;

	for (size_t i = 0; i < m_EnemyType.Behaviours.size(); i++)
	{
		AddActingBehaviour(*m_EnemyType.Behaviours[i]);
	}

	auto explosion = CreateExplosion("explosion.png");
	m_EnemyInstanceData.Scene->AddGameObjectHandle(explosion);

	CEnemyFormation* enemyManager = m_EnemyInstanceData.EnemyManager;

	OnDie().Subscribe([enemy, enemyManager, explosion, dieAudio]()
		{
			dieAudio->Play();
			explosion->GetComponent<CLifeTime>()->Respawn(enemy->Transform().GetWorldTransform().Position);
			enemy->GetComponent<CStatController>()->ResetCurrentStat(StatType::Points);
			enemy->SetActive(false);
			enemyManager->SendNextTroops(enemy.get());
		});
	
}

dae::GameObject& dae::Enemy::GetGameObject()
{
	return *m_Self.get();
}

dae::GameObjectHandle dae::Enemy::GetGameObjectHandle() const
{
	return m_Self;
}

void dae::Enemy::SetStartingFormationBehaviour(EnemyBehaviour& enemyBehaviour)
{
	auto onEnd = m_OnEndStartingFormationSequence;
	auto self = m_Self;
	m_StartFormationSequence = enemyBehaviour.CreateInstance(*this);
	m_StartFormationSequence->AddEndSubAction([onEnd, self]() {onEnd->Invoke(*self); });
}

void dae::Enemy::SubscribeOnEndStartingFormationBehaviour(const std::function<void(GameObject&)>& func)
{
	m_OnEndStartingFormationSequence->Subscribe(func);
}

void dae::Enemy::AddActingBehaviour(EnemyBehaviour& enemyBehaviour)
{
	MovementActionSequenceHandle sequence{ enemyBehaviour.CreateInstance(*this) };

	GameObjectHandle self = m_Self;

	IntStat pointsInFormation = m_PointsInFormation;

	CEnemyFormation* enemyManager = m_EnemyInstanceData.EnemyManager;

	sequence->AddEndSubAction([self, pointsInFormation, enemyManager]() mutable 
		{

			self->GetComponent<CStatController>()->CreateStat(StatType::Points, pointsInFormation);
			enemyManager->SendNextTroops(self.get());

		});

	m_EnemyActingSequences.push_back(sequence);
}

bool dae::Enemy::StartFormation()
{
	return m_StartFormationSequence->RestartSequence();
}

const dae::CMovementActionSequence& dae::Enemy::Act()
{
	int chosenIndex = Random::GetRandomBetweenRange(0, (int)m_EnemyActingSequences.size() - 1);

	if (!m_EnemyActingSequences[chosenIndex]->RestartSequence())
	{
		chosenIndex = 0;
		m_EnemyActingSequences[chosenIndex]->RestartSequence();
	}

	m_Self->GetComponent<CStatController>()->CreateStat(StatType::Points, m_PointsWhileDiving);

	m_CurrentEnemyActingSequence = chosenIndex;

	m_OnActAudio->Play();

	return *m_EnemyActingSequences[chosenIndex];
}

bool dae::Enemy::IsActing()const {

	return m_EnemyActingSequences[m_CurrentEnemyActingSequence]->IsActing();
}

void dae::Enemy::StopActing()
{
	m_EnemyActingSequences[m_CurrentEnemyActingSequence]->StopSequence();
}

const dae::EnemyInstanceData& dae::Enemy::GetEnemyInstanceData() const
{
	return m_EnemyInstanceData;
}

const dae::EnemyType& dae::Enemy::GetEnemyType() const
{
	return m_EnemyType;
}

dae::Event<>& dae::Enemy::OnDie()
{
	return m_Self->GetComponent<CStatController>()->OnCurrentStatZeroOrLess(StatType::Health);
}
