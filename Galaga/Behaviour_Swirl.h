#pragma once

#include "Enemy.h"
#include "CMovementActionSequence.h"
#include "SwirlData.h"



namespace dae {

	class SwirlBehaviour final : public EnemyBehaviour
	{
	public:
		SwirlBehaviour(SwirlData swirlData) :
			m_SwirlData(swirlData) {}

		MovementActionSequenceHandle CreateInstance(Enemy& enemyCreator) override;

	private:
		SwirlData m_SwirlData{};

	};

}