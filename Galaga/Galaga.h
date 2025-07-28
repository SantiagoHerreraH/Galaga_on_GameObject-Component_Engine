#pragma once

#include "RoundManager.h"
#include "PlayerVsScene.h"
#include "MainMenu.h"

namespace dae {

	void Galaga() {

		MainMenu mainMenu{};
		RoundManager onePlayerVsNPCs{"ONE"};
		RoundManager twoPlayersVsNPCs{"TWO"};
		PlayerVsScene playerVsScene{};

		mainMenu.AddSceneChangeButton(onePlayerVsNPCs.GetFirstRoundName(), onePlayerVsNPCs.GetFileName());
		mainMenu.AddSceneChangeButton(twoPlayersVsNPCs.GetFirstRoundName(), twoPlayersVsNPCs.GetFileName());
		mainMenu.AddSceneChangeButton(playerVsScene.GetSceneName());
	}

}