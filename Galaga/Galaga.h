#pragma once

#include "RoundManager.h"
#include "PlayerVsScene.h"
#include "MainMenu.h"
#include "DefineData_EnemyGridData.h"
#include "DefineData_EnemyType.h"
#include "DefineData_SwirlData.h"

namespace dae {

	void Galaga() {

		DefineEnemyGridData::DefineTypes();
		DefineEnemyType::DefineTypes();
		DefineSwirlData::DefineTypes();

		MainMenu mainMenu{};
		RoundManager onePlayerVsNPCs{"GameModes/OnePlayerVsEnemies.txt"};
		RoundManager twoPlayersVsNPCs{"GameModes/TwoPlayersVsEnemies.txt"};
		PlayerVsScene playerVsScene{};

		mainMenu.AddSceneChangeButton(onePlayerVsNPCs.GetFirstRoundName(), "ONE");
		mainMenu.AddSceneChangeButton(twoPlayersVsNPCs.GetFirstRoundName(), "TWO");
		mainMenu.AddSceneChangeButton(playerVsScene.GetSceneName());
	}

}