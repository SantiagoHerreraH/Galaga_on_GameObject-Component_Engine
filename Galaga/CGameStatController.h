#pragma once

#include "CStatController.h"

namespace dae {

	using GameStat = Stat<int>;

	enum class GameStatType {
		Health,
		Points,
		ShotsFired,
		NumberOfHits
	};

	using CGameStatController = CStatController<GameStatType, int>;
}