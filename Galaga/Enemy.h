#pragma once
#include <functional>
#include "Scene.h"
#include "EnemyInstanceData.h"
#include "GalagaStats.h"
#include "MovementActionSequence.h"
#include "SwirlData.h"

namespace dae {

	class Enemy;

	using MovementActionSequenceHandle = std::shared_ptr<MovementActionSequence>;

	class EnemyBehaviour {

	public:
		virtual ~EnemyBehaviour() = default;
		virtual MovementActionSequenceHandle CreateInstance(Enemy& enemy) = 0;
	};

	struct EnemyType {
		std::string TextureName;
		int MaxHealth;
		int PointsInFormation;
		int PointOnDive;
		std::vector<std::shared_ptr<EnemyBehaviour>> Behaviours;
	};

	class Enemy final{

	public:
		Enemy(const EnemyInstanceData& enemyInstanceData, const EnemyType& enemyType);

		GameObject& GetGameObject();
		GameObjectHandle GetGameObjectHandle()const;

		void SetStartingFormationBehaviour(EnemyBehaviour& enemyBehaviour);
		void AddActingBehaviour(EnemyBehaviour& enemyBehaviour);

		bool StartFormation();
		const MovementActionSequence& Act();
		bool IsActing()const;
		void StopActing();

		const EnemyInstanceData& GetEnemyInstanceData()const;
		const EnemyType& GetEnemyType()const;
		
		Event<>& OnDie();

	private:

		GameObjectHandle m_Self;

		EnemyInstanceData m_EnemyInstanceData;
		EnemyType m_EnemyType;
		std::shared_ptr<MovementActionSequence> m_StartFormationSequence;
		std::vector<std::shared_ptr<MovementActionSequence>> m_EnemyActingSequences;
		int m_CurrentEnemyActingSequence{0};

		Event<GameObject&> m_OnEndAction;

		IntStat m_PointsWhileDiving;
		IntStat m_PointsInFormation;

	};
}