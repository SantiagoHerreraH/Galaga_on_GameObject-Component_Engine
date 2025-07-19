#pragma once

#include "StatSystem.h"

namespace dae {

	using IntStat = Stat<int>;

	enum class StatType {
		Health,
		Points,
		ShotsFired,
		NumberOfHits
	};

	using CStatController = StatController<StatType, int>;
}