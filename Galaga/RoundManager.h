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
	};

	void from_json(const nlohmann::json& j, RoundManagerType& roundManagerType) {

		j.at("GameRounds").get_to(roundManagerType.GameRounds);
	}

	class RoundManager
	{
	public:
		RoundManager(const std::string& fileName);

	private:
		bool LoadRoundManagerType(const std::string& fileName);
		void CreateRounds();
		void CreateHighscoreScene();
		void CreatePlayer();
		void CreateParticleSystem();

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

			RoundManagerType RoundManagerType;
			size_t CurrentRoundIdx{ 0 };
			GalagaPlayer Player;
			std::vector<std::string> SceneNames;
			std::string HighscoreSceneName;
			GameObjectHandle ParticleSystemGameObj;
		};

		std::shared_ptr<RoundManagerData> m_RoundManagerData;

	};

	

}
