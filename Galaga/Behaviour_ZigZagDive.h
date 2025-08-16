#pragma once

#include "Enemy.h"
#include "CMovementActionSequence.h"


namespace dae {


	class ZigZagDiveBehaviour final : public EnemyBehaviour {

	public:

		virtual MovementActionSequenceHandle CreateInstance(Enemy& enemyCreator) override;

	};
}