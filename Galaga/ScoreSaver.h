#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <filesystem> 


namespace dae {

    struct ScoreData {
        std::string Name;
        int Score;
    };

    class ScoreSaver final
    {
    public:
        ScoreSaver(const std::string& filename);
        ~ScoreSaver();
        
        void AddScore(const ScoreData& score);
        int GetScore(const std::string& name);

        ScoreData GetHighscore();

        void Save();

    private:
        bool m_WasModified{false};
        ScoreData m_HighScore;
        ScoreData m_CurrentScore;
        std::filesystem::path m_DataPath;
        std::string m_CompleteFilename;
        std::unordered_map<std::string, int> m_Scores;
    };
    
    
}

