#ifndef SCOREMANAGER_HPP
#define SCOREMANAGER_HPP

#include <string>

class ScoreManager {
public:
    static ScoreManager& getInstance();

    bool loadHighScore();
    void saveHighScore(int score);
    int getHighScore() const { return highScore; }

private:
    ScoreManager();
    ~ScoreManager();

    ScoreManager(const ScoreManager&) = delete;
    ScoreManager& operator=(const ScoreManager&) = delete;

    std::string getScoreFilePath() const;

    int highScore;
    static const char* SCORE_FILE;
};

#endif
