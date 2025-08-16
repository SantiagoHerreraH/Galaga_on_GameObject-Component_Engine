#pragma once

#include "EnemyGridData.h"
#include "StringToDataModifier.h"

namespace dae {


	using StringToEnemyGridData = StringToDataModifier<EnemyGridData>;

	class DefineData_EnemyGridData {

	public:
		static void DefineTypes() {

			StringToEnemyGridData::AddDataModificationFunction("SmallGrid", SetSmallGrid);
			StringToEnemyGridData::AddDataModificationFunction("MediumGrid", SetMediumGrid);
			StringToEnemyGridData::AddDataModificationFunction("LargeGrid", SetLargeGrid);

		}

		static void SetSmallGrid(EnemyGridData& data) {
			data.SpaceBetweenEnemies = 50;
			data.SpaceBetweenRows = 50;
			data.NumberOfEnemiesPerRow = 4;
			data.NumberOfEnemyRows = 1;
		}
		static void SetMediumGrid(EnemyGridData& data) {
			data.SpaceBetweenEnemies = 50;
			data.SpaceBetweenRows = 50;
			data.NumberOfEnemiesPerRow = 4;
			data.NumberOfEnemyRows = 2;
		}
		static void SetLargeGrid(EnemyGridData& data) {
			data.SpaceBetweenEnemies = 50;
			data.SpaceBetweenRows = 50;
			data.NumberOfEnemiesPerRow = 8;
			data.NumberOfEnemyRows = 2;
		}
	};



}