#pragma once

#include "EnemyFormation.h"
#include "Player.h"


namespace dae {


	struct RoundType {
		EnemyFormationType FirstEnemyFormation;
		EnemyFormationType SecondEnemyFormation;
		EnemyFormationType ThirdEnemyFormation;
	};

	void from_json(const nlohmann::json& j, RoundType& roundType) {

		j.at("FirstFormation").get_to(roundType.FirstEnemyFormation);
		j.at("SecondFormation").get_to(roundType.SecondEnemyFormation);
		j.at("ThirdFormation").get_to(roundType.ThirdEnemyFormation);
	}

	struct RoundManagerType {

		std::vector<RoundType> GameRounds;
		int PlayerCount = 1;
	};

	void from_json(const nlohmann::json& j, RoundManagerType& roundManagerType) {

		j.at("GameRounds").get_to(roundManagerType.GameRounds);
		j.at("PlayerCount").get_to(roundManagerType.PlayerCount);
	}

	class RoundManager
	{
	public:
		RoundManager(const std::string& fileName);

	private:
		bool LoadRoundManagerType(const std::string& fileName);
		void CreateRounds();
		void CreateHighscoreScene();
		void CreatePlayers();

		struct RoundManagerData {

			const RoundType& GetCurrentRoundType() {
				return RoundManagerType.GameRounds[CurrentRoundIdx];
			}

			void IncreaseRoundNum() {

				++CurrentRoundIdx; 
			}

			void ResetRoundNum() {
				CurrentRoundIdx = 0;
			}

			bool CanIncreaseRoundNum()const {
				return CurrentRoundIdx < (RoundManagerType.GameRounds.size() - 1);
			}

			GalagaPlayer& GetRandomPlayer() {
				return Players[Random::GetRandomBetweenRange(0, (int)Players.size())];
			}

			RoundManagerType RoundManagerType;
			size_t CurrentRoundIdx{ 0 };
			std::vector<GalagaPlayer> Players;
			int PlayerDeaths;
			std::vector<std::string> SceneNames;
			std::string HighscoreSceneName;
			GameObjectHandle ParticleSystemGameObj;
		};

		std::shared_ptr<RoundManagerData> m_RoundManagerData;

	};

	

}
