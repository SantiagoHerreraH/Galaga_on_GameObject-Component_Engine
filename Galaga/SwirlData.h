#pragma once

#include <glm.hpp>
#include "Settings.h"
#include <vector>

namespace dae {

	struct SwirlData {

		glm::ivec2 StartWorldPos{};
		glm::vec2 PreSwirlMovementDirection{};
		float PreSwirlGoalDistance{ 100 };
		float PreSwirlSpeed{ 100 };//m/s

		glm::vec2 SwirlStartPointRelativeToCenter{ 0,1 };//starting point -> if y == 1 starts at bottom 
		float SwirlTimeMultiplier{ 1 };
		float SwirlSpeed{ 300 };
		float SwirlOrthogonalMovementRadius{ 30 };
		float SwirlPerpendicularMovementRadius{ 30 };
		float SwirlNumberOfCircles{ 1 };

		float PostSwirlSpeedToReturn{ 50 };
		float PostSwirlRadiusToAttach{ 10 };
	};

	struct EnemyUnitSwirlData {
		std::vector<SwirlData> SwirlDataPerUnit{};
		int EnemiesInAUnit{ 4 };
	};

	


}