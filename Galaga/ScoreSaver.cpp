#include "ScoreSaver.h"
#include "ResourceManager.h"

namespace dae {
    
    ScoreSaver::ScoreSaver(const std::string& filename)
    {
        const auto fullPath = ResourceManager::GetInstance().GetDataPath() / filename;
        m_CompleteFilename = fullPath.string();
        m_HighScore.PlayerName = "None";
        m_HighScore.Score = 0;

        std::ifstream in_file(m_CompleteFilename);
        if (!in_file.is_open()) {
            std::cerr << "Failed to open " + m_CompleteFilename + " for reading\n";
            return;
        }

        if (in_file.peek() == std::ifstream::traits_type::eof()) return;

        nlohmann::json j_read;
        in_file >> j_read;

        m_Scores = j_read.get<std::vector<ScoreData>>();

        for (auto& score : m_Scores)
        {
            m_GameModeNameToLastScore[score.GameModeName] = score.Score;

            if (m_HighScore.Score < score.Score)
            {
                m_HighScore = score;
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
        m_Scores.push_back(score);

        if (m_HighScore.Score < score.Score)
        {
            m_HighScore.Score = score.Score;
            m_HighScore.PlayerName = score.PlayerName;
        }

        m_GameModeNameToLastScore[score.GameModeName] = score.Score;
    }

    ScoreData ScoreSaver::GetHighscore()
    {
        return m_HighScore;
    }

    int ScoreSaver::GetLastScore(const std::string& gameModeName)
    {
        if (m_GameModeNameToLastScore.contains(gameModeName))
        {
            return m_GameModeNameToLastScore[gameModeName];
        }

        return 0;
    }

    ScoreData ScoreSaver::GetHighscoreInTheLastScores(const std::string& gameModeName, int amountOfLastScores)
    {
        if (m_Scores.empty())
        {
            return ScoreData{};
        }
        int startIdx = m_Scores.size() - 1;
        startIdx = startIdx < 0 ? 0 : startIdx;

        ScoreData scoreData{ m_Scores[startIdx] };
        scoreData.Score = 0;

        int currentAmountOfScores = 0;
        int currentIdx = startIdx;

        while (currentAmountOfScores < amountOfLastScores && currentIdx >= 0)
        {
            if (m_Scores[currentIdx].GameModeName == gameModeName)
            {
                ++currentAmountOfScores;
                if (scoreData.Score < m_Scores[currentIdx].Score)
                {
                    scoreData = m_Scores[currentIdx];
                }
            }

            --currentIdx;
        }

        return scoreData;
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