#pragma once

#include "CEnemyFormation.h"
#include "Player.h"


namespace dae {

	class Scene_Highscore;

	struct RoundType {
		EnemyFormationType FirstEnemyFormation;
		EnemyFormationType SecondEnemyFormation;
		EnemyFormationType ThirdEnemyFormation;
	};

	inline void from_json(const nlohmann::json& j, RoundType& roundType) {

		j.at("FirstFormation").get_to(roundType.FirstEnemyFormation);
		j.at("SecondFormation").get_to(roundType.SecondEnemyFormation);
		j.at("ThirdFormation").get_to(roundType.ThirdEnemyFormation);
	}

	struct RoundManagerType {

		std::vector<RoundType> GameRounds;
		int PlayerCount = 1;
	};

	/*
	
	
	
	*/

	inline void from_json(const nlohmann::json& j, RoundManagerType& roundManagerType) {

		j.at("GameRounds").get_to(roundManagerType.GameRounds);
		j.at("PlayerCount").get_to(roundManagerType.PlayerCount);
	}

	class RoundManager final
	{
	public:
		RoundManager(const std::string& fileName);
		std::string GetNameAssignerSceneName()const;
		const std::string& GetFileName()const;

	private:
		bool LoadRoundManagerType(const std::string& fileName);
		void CreateRounds();
		void CreateHighscoreScene();
		void CreatePlayers();
		void CreateNameAssignerScene();

		struct RoundManagerData {

			RoundManagerData(const std::string& fileName) : FileName(fileName){}

			const RoundType& GetCurrentRoundType() {
				return RoundManagerType.GameRounds[CurrentRoundIdx];
			}

			std::string GetRoundName(int roundIdx)
			{
				return FileName + " Round " + std::to_string(roundIdx + 1);
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

			Player& GetRandomPlayer() {
				return Players[Random::GetRandomBetweenRange(0, (int)(Players.size() - 1))];
			}

			RoundManagerType RoundManagerType;
			size_t CurrentRoundIdx{ 0 };
			std::vector<Player> Players;
			std::vector<std::string> SceneNames;
			std::string HighscoreSceneName;
			GameObjectHandle ParticleSystemGameObj;
			std::string FileName;
		};


		std::string m_FileName;
		std::shared_ptr<RoundManagerData> m_RoundManagerData;

	};

	

}
