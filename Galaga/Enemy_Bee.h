#pragma once

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
#include "Enemy.h"

namespace dae {

	GameObjectHandle CreateBeeEnemy(GameObjectHandle grid, dae::Scene& scene, const glm::vec3& relativePos, GameObjectHandle player, Event<>& onAct, std::shared_ptr<Event<GameObject&>> onEndAction, std::shared_ptr<Event<GameObject&>> onDie, std::shared_ptr<CMovementActionSequence>chosenMovementAction, bool goTowardsLeft) 
	{
		//------

		SpriteSheet spriteSheet{ dae::CTextureHandle{"zako.png"}, 1, 2 };
		spriteSheet.TextureHandle().Center();

		CAnimation animation{};
		animation.SetFramesPerSecond(2);
		animation.AddFrame(spriteSheet.GetFrame(0, 0), 1);
		animation.AddFrame(spriteSheet.GetFrame(0, 1), 1);

		//------

		Rect rect{};
		rect.Height = 32;
		rect.Width = 32;
		CCollider collider{rect, (int)GalagaCollisionLayers::Enemies};

		collider.OnCollisionBeginEvent().Subscribe([player](GameObject&, GameObject& other) mutable
			{
				if (player.get() == &other)
				{
					player->GetComponent<CStatController>()->OffsetStat(StatType::Health, -1);
				}
			});

		//------

		CStatController statController{};

		IntStat initialHealth{ 1, 1, 1 };
		IntStat initialKills{ 0, 0, 0 };

		statController.CreateStat(StatType::Health, initialHealth);


		//------ CREATE GAMEOBJECT

		GameObjectHandle enemy{ scene.CreateGameObject() };// , transformData, animation, rectTrigger, statController
		enemy->Transform().SetLocalPosition(relativePos);
		enemy->AddComponent(animation);
		enemy->AddComponent(collider);
		enemy->AddComponent(statController);

		enemy->Transform().SetParent(*grid, ETransformReparentType::KeepLocalTransform);
		enemy->Transform().OverrideWorldScaleWithLocalScale(true);

		auto wrappedOnDie = [onDie, enemy]() mutable {

			onDie->Invoke(*enemy);

			};

		enemy->GetComponent<CStatController>()->OnCurrentStatZeroOrLess(StatType::Health).Subscribe(wrappedOnDie);


		//-------- POINTS

		const float showPointsDuration{ 1.f };

		//-- POINTS IN FORMATION

		TimerSystem* TIMERSYSTEM = &TimerSystem::GetFromScene(&scene);

		IntStat pointsInFormation{ 50, 50, 50 };
		GameObjectHandle pointsInFormationIndicator{ CreatePointIndicator(scene, pointsInFormation.CurrentStat) };
		TimerKey showPointsInFormationKey = TIMERSYSTEM->StartEndFunctionWithDuration(
			[pointsInFormationIndicator, enemy]() mutable {

				pointsInFormationIndicator->Transform().SetLocalPosition(enemy->Transform().GetWorldTransform().Position);
				pointsInFormationIndicator->SetActive(true); },
			[pointsInFormationIndicator]() mutable {pointsInFormationIndicator->SetActive(false);  },
			showPointsDuration,
			false,
			true
		);

		pointsInFormation.OnResetCurrentStat.Subscribe([TIMERSYSTEM, showPointsInFormationKey]() {

			TIMERSYSTEM->RestartTimer(showPointsInFormationKey);

			});

		//-- POINTS WHILE DIVING

		IntStat pointsWhileDiving{ 100, 100, 100 };
		GameObjectHandle pointsWhileDivingIndicator{ CreatePointIndicator(scene, pointsWhileDiving.CurrentStat) };
		TimerKey showPointsWhileDivingKey = TIMERSYSTEM->StartEndFunctionWithDuration(
			[pointsWhileDivingIndicator, enemy]() mutable {
				pointsWhileDivingIndicator->Transform().SetLocalPosition(enemy->Transform().GetWorldTransform().Position);
				pointsWhileDivingIndicator->SetActive(true); },
			[pointsWhileDivingIndicator]() mutable {pointsWhileDivingIndicator->SetActive(false); },
			showPointsDuration,
			false,
			true
		);

		pointsWhileDiving.OnResetCurrentStat.Subscribe([showPointsWhileDivingKey, TIMERSYSTEM]() {

			TIMERSYSTEM->RestartTimer(showPointsWhileDivingKey);

			});

		enemy->GetComponent<CStatController>()->CreateStat(StatType::Points, pointsInFormation);

		//------ Behaviours
		std::vector<CMovementActionSequence> behaviours{};
		behaviours.push_back(AddUDiveBehavior(scene, enemy, grid, player, relativePos, onEndAction, goTowardsLeft));

		onDie->Subscribe([](GameObject& self) {
			self.GetComponent<CStatController>()->ResetCurrentStat(StatType::Points);
			});

		onEndAction->Subscribe([enemy, pointsInFormation](GameObject&) mutable {

			enemy->GetComponent<CStatController>()->CreateStat(StatType::Points, pointsInFormation);

			});

		onAct.Subscribe([behaviours, chosenMovementAction, enemy, pointsWhileDiving]() mutable {

			int chosenIndex = Random::GetRandomBetweenRange(0, (int)behaviours.size() - 1);

			if (!behaviours.at(chosenIndex).RestartSequence())
			{
				chosenIndex = 0;
				behaviours.at(chosenIndex).RestartSequence();
			}

			(*chosenMovementAction) = behaviours.at(chosenIndex);

			enemy->GetComponent<CStatController>()->CreateStat(StatType::Points, pointsWhileDiving);

			});

		return enemy;


	}

}