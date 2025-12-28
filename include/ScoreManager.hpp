#ifndef SCOREMANAGER_HPP
#define SCOREMANAGER_HPP

#include <string>
#include "Menu.hpp"

class ScoreManager {
public:
    static ScoreManager& getInstance();

    bool loadHighScore();
    void saveHighScore(int score, Difficulty difficulty);
    int getHighScore(Difficulty difficulty) const;

private:
    ScoreManager();
    ~ScoreManager();

    ScoreManager(const ScoreManager&) = delete;
    ScoreManager& operator=(const ScoreManager&) = delete;

    std::string getScoreFilePath() const;

    int highScoreEasy;
    int highScoreMedium;
    int highScoreHard;
    static const char* SCORE_FILE;
};

#endif
