#pragma once

#include "RoundManager.h"
#include "Scene_PlayerVs.h"
#include "Scene_MainMenu.h"
#include "DefineData_EnemyGridData.h"
#include "DefineData_EnemyType.h"
#include "DefineData_SwirlData.h"

namespace dae {

	void Galaga() {

		DefineData_EnemyGridData::DefineTypes();
		DefineData_EnemyType::DefineTypes();
		DefineData_SwirlData::DefineTypes();

		Scene_MainMenu mainMenu{};
		RoundManager onePlayerVsNPCs{"GameModes/OnePlayerVsEnemies.txt"};
		RoundManager twoPlayersVsNPCs{"GameModes/TwoPlayersVsEnemies.txt"};
		Scene_PlayerVs playerVsScene{};

		mainMenu.AddSceneChangeButton(onePlayerVsNPCs.GetNameAssignerSceneName(), "ONE");
		mainMenu.AddSceneChangeButton(twoPlayersVsNPCs.GetNameAssignerSceneName(), "TWO");
		mainMenu.AddSceneChangeButton(playerVsScene.GetNameAssignerSceneName(), "VERSUS");
	}

}