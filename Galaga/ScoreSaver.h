#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem> 
#include <unordered_map>
#include "nlohmann/json.hpp"


namespace dae {

    struct ScoreData {

        std::string GameModeName;
        std::string PlayerName;
        int Score;
    };

    inline void from_json(const nlohmann::json& j, ScoreData& scoreData) {

        j.at("GameModeName").get_to(scoreData.GameModeName);
        j.at("PlayerName").get_to(scoreData.PlayerName);
        j.at("Score").get_to(scoreData.Score);
    }

    inline void to_json(nlohmann::json& j, const ScoreData& scoreData) {
        j = nlohmann::json{ 
            {"GameModeName", scoreData.GameModeName},
            {"PlayerName", scoreData.PlayerName},
            {"Score", scoreData.Score}};
    }

    class ScoreSaver final
    {
    public:
        ScoreSaver(const std::string& filename = "Highscores/HighScoreData.txt");
        ~ScoreSaver();
        
        void AddScore(const ScoreData& score);

        ScoreData GetHighscore();
        int GetLastScore(const std::string& gameModeName);
        ScoreData GetHighscoreInTheLastScores(const std::string& gameModeName, int amountOfLastScores);

        void Save();

    private:
        bool m_WasModified{false};
        ScoreData m_HighScore;
        std::unordered_map<std::string, int> m_GameModeNameToLastScore;
        std::filesystem::path m_DataPath;
        std::string m_CompleteFilename;
        std::vector<ScoreData> m_Scores;
    };
    
    
}

