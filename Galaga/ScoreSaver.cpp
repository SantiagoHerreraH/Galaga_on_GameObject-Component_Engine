#include "ScoreSaver.h"
#include "ResourceManager.h"
#include "nlohmann/json.hpp"

namespace dae {
    
    ScoreSaver::ScoreSaver(const std::string& filename)
    {
        const auto fullPath = ResourceManager::GetInstance().GetDataPath() / filename;
        m_CompleteFilename = std::filesystem::path(fullPath).filename().string();

        m_HighScore.Name = "None";
        m_HighScore.Score = -1;

        std::ifstream in_file(m_CompleteFilename);
        if (!in_file.is_open()) {
            std::cerr << "Failed to open " + m_CompleteFilename + " for reading\n";
            return;
        }

        nlohmann::json j_read;
        in_file >> j_read;

        m_Scores = j_read.get<std::unordered_map<std::string, int>>();

        for (auto& score : m_Scores)
        {
            if (m_HighScore.Score < score.second)
            {
                m_HighScore.Score = score.second;
                m_HighScore.Name = score.first;
            }
        }
    }

    ScoreSaver::~ScoreSaver()
    {
        Save();
    }

    void ScoreSaver::AddScore(const ScoreData& score)
    {
        m_WasModified = true;
        m_Scores[score.Name] = score.Score;

        if (m_HighScore.Score < score.Score)
        {
            m_HighScore.Score = score.Score;
            m_HighScore.Name = score.Name;
        }
    }

    int ScoreSaver::GetScore(const std::string& name)
    {
        if (m_Scores.contains(name))
        {
            return m_Scores[name];
        }
        return -1;
    }

    ScoreData ScoreSaver::GetHighscore()
    {
        return m_HighScore;
    }

    void ScoreSaver::Save()
    {
        if (!m_WasModified)
        {
            return;
        }
        // Step 2: Convert map to JSON
        nlohmann::json j = m_Scores;

        // Step 3: Write JSON to file
        std::ofstream out_file(m_CompleteFilename);
        if (!out_file.is_open()) {
            std::cerr << "Failed to open " + m_CompleteFilename +  " for writing\n";
            return;
        }
        out_file << j.dump(4);  // Pretty-print with 4-space indent
        out_file.close();
    }

}