#pragma once


#define _USE_MATH_DEFINES
#include <cmath>
#include "Enemy.h"
#include "MovementActionSequence.h"
#include "Scene.h"
#include "SwirlData.h"
#include "WaveMovement.h"
#include "GameTime.h"
#include "Animation.h"
#include "Collider.h"
#include "CollisionLayers.h"
#include "GalagaStats.h"
#include "Settings.h"
#include "MathTools.h"
#include "WeaponType.h"

namespace dae {

	class CaptureZoneBehaviour final : public EnemyBehaviour {

	public:
		MovementActionSequenceHandle CreateInstance(Enemy& enemyCreator) override;

	};

}