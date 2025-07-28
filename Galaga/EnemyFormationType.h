#pragma once

#include "nlohmann/json.hpp"
#include <vector>
#include "SwirlData.h"
#include "EnemyGridData.h"
#include "Enemy.h"
#include "DefineData_EnemyGridData.h"
#include "DefineData_EnemyType.h"
#include "DefineData_SwirlData.h"

namespace dae {

	struct EnemyFormationType {

		EnemyUnitSwirlData EnemyUnitSwirlData; // list of names
		EnemyGridData EnemyGridData; // name : small, medium, large
		EnemyType EnemyType; //name : Bee, Bird, Butterfly
	};

	inline void from_json(const nlohmann::json& j, EnemyFormationType& managerType) {

		std::string gridType = j.at("GridType");
		std::string enemyType = j.at("EnemyType");
		std::vector<std::string> swirlFormations = j.at("SwirlFormations");
		
		StringToEnemyGridData::ApplyDataModification(gridType, managerType.EnemyGridData);
		StringToEnemyType::ApplyDataModification(gridType, managerType.EnemyType);

		managerType.EnemyUnitSwirlData.SwirlDataPerUnit.clear();
		for (size_t i = 0; i < swirlFormations.size(); i++)
		{
			StringToSwirlType::ApplyDataModification(swirlFormations[i], managerType.EnemyUnitSwirlData);
		}
	}
}