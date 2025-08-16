#pragma once

#include "Enemy.h"
#include "CMovementActionSequence.h"

namespace dae {

	class CaptureZoneBehaviour final : public EnemyBehaviour {

	public:
		MovementActionSequenceHandle CreateInstance(Enemy& enemyCreator) override;

	};

}