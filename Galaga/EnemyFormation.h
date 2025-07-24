#pragma once

#include <algorithm>
#include "Scene.h"
#include "RoundUI.h"
#include "MovementActionSequence.h"
#include "SwirlData.h"
#include "Behaviour_Swirl.h"
#include "Random.hpp"
#include "EnemyFormationType.h"
#include "Player.h"

namespace dae {
	
	class CEnemyFormation final: public Component {

	public:
		CEnemyFormation(
			const glm::vec2& position,
			float secondsBetweenSpawning,
			const GalagaPlayer& player,
			const EnemyFormationType& enemyFormationType);

		virtual void Start() override;

		void StartEnemySwirlBehaviour(bool startActingBehaviourAfterEndSwirls = true);

		void StopSendingTroops();
		void StartSendingTroops();
		bool SendNextTroops(GameObject* incomingTroop);

		void SubscribeOnFormationDeath(const std::function<void()>& func);
		void SubscribeOnEndFormationSwirlBehaviour(const std::function<void()>& func);

	private:
		
		void CreatePlayerReactions();
		void CreateFormationTimer();
		void CreateGrid();
		void CreateEnemies();
		void CheckEnemyDeaths();
		std::vector<Enemy> m_Enemies;

		bool m_CanSendTroops{true};

		float m_SecondsBetweenSpawning{ 1.f };
		glm::vec2 m_StartPosition;
		EnemyUnitSwirlData m_EnemyUnitSwirlData;
		EnemyGridData m_EnemyGridData;
		GameObjectHandle m_Grid;
		GalagaPlayer m_Player;
		EnemyType m_EnemyType;

		Event<> m_OnFormationDeath;
		Event<> m_OnEndSwirlFormation;

		TimerKey m_FormationTimerKey;
		bool m_CreatedFormationTimer{ false };
		bool m_CalledStartFormationBeforeStart{ false };

	};

}