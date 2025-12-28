#include "ScoreManager.hpp"
#include <fstream>
#include <iostream>

const char* ScoreManager::SCORE_FILE = "amazing_ball_highscore.dat";

ScoreManager::ScoreManager() : highScoreEasy(0), highScoreMedium(0), highScoreHard(0) {
    loadHighScore();
}

ScoreManager::~ScoreManager() {
}

ScoreManager& ScoreManager::getInstance() {
    static ScoreManager instance;
    return instance;
}

std::string ScoreManager::getScoreFilePath() const {
    // Sauvegarder dans le répertoire courant
    return SCORE_FILE;
}

bool ScoreManager::loadHighScore() {
    std::ifstream file(getScoreFilePath());

    if (!file.is_open()) {
        // Fichier n'existe pas encore, c'est normal au premier lancement
        highScoreEasy = 0;
        highScoreMedium = 0;
        highScoreHard = 0;
        return false;
    }

    file >> highScoreEasy >> highScoreMedium >> highScoreHard;
    file.close();

    // Validation des scores chargés
    if (highScoreEasy < 0) highScoreEasy = 0;
    if (highScoreMedium < 0) highScoreMedium = 0;
    if (highScoreHard < 0) highScoreHard = 0;

    return true;
}

void ScoreManager::saveHighScore(int score, Difficulty difficulty) {
    // Déterminer quel score mettre à jour
    int* targetScore = nullptr;
    switch (difficulty) {
        case Difficulty::EASY:
            targetScore = &highScoreEasy;
            break;
        case Difficulty::MEDIUM:
            targetScore = &highScoreMedium;
            break;
        case Difficulty::HARD:
            targetScore = &highScoreHard;
            break;
    }

    // Ne sauvegarder que si c'est un nouveau record pour cette difficulté
    if (targetScore && score > *targetScore) {
        *targetScore = score;

        std::ofstream file(getScoreFilePath());

        if (!file.is_open()) {
            std::cerr << "Erreur: Impossible de sauvegarder le score dans " << getScoreFilePath() << std::endl;
            return;
        }

        file << highScoreEasy << " " << highScoreMedium << " " << highScoreHard;
        file.close();
    }
}

int ScoreManager::getHighScore(Difficulty difficulty) const {
    switch (difficulty) {
        case Difficulty::EASY:
            return highScoreEasy;
        case Difficulty::MEDIUM:
            return highScoreMedium;
        case Difficulty::HARD:
            return highScoreHard;
        default:
            return 0;
    }
}
