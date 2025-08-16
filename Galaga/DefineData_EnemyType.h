#pragma once

#include "StringToDataModifier.h"
#include "Enemy.h"
#include "Behaviour_CaptureZone.h"
#include "Behaviour_UDive.h"
#include "Behaviour_ZigZagDive.h"

namespace dae {

	using StringToEnemyType = StringToDataModifier<EnemyType>;


	class DefineData_EnemyType {

	public:

		static void DefineTypes() {

			StringToEnemyType::AddDataModificationFunction("Butterfly", CreateButterflyEnemy);
			StringToEnemyType::AddDataModificationFunction("Bee", CreateBeeEnemy);
			StringToEnemyType::AddDataModificationFunction("Bird", CreateBirdEnemy);
		}

		static void CreateButterflyEnemy(EnemyType& enemyType) {

			enemyType.TextureName = "goei.png";
			enemyType.MaxHealth = 1;
			enemyType.PointsInFormation = 80;
			enemyType.PointOnDive = 160;

			enemyType.Behaviours.clear();
			enemyType.Behaviours.push_back(std::make_shared<ZigZagDiveBehaviour>());
		}

		static void CreateBeeEnemy(EnemyType& enemyType) {

			enemyType.TextureName = "zako.png";
			enemyType.MaxHealth = 1;
			enemyType.PointsInFormation = 50;
			enemyType.PointOnDive = 100;

			enemyType.Behaviours.clear();
			enemyType.Behaviours.push_back(std::make_shared<UDiveBehaviour>());
		}

		static void CreateBirdEnemy(EnemyType& enemyType) {

			enemyType.TextureName = "boss.png";
			enemyType.SecondStageTextureName = "boss2.png";
			enemyType.MaxHealth = 2;
			enemyType.PointsInFormation = 150;
			enemyType.PointOnDive = 400;
			enemyType.Behaviours.clear();
			enemyType.Behaviours.push_back(std::make_shared<UDiveBehaviour>());
			enemyType.Behaviours.push_back(std::make_shared<ZigZagDiveBehaviour>());
			enemyType.Behaviours.push_back(std::make_shared<CaptureZoneBehaviour>());
		}
	};
}