#pragma once

namespace dae{

	struct EnemyGridData {
		int NumberOfEnemyRows;
		int NumberOfEnemiesPerRow;
		float SpaceBetweenEnemies{ 50 };
		float SpaceBetweenRows{ 50 };
	};
}