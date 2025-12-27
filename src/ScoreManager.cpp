#include "ScoreManager.hpp"
#include <fstream>
#include <iostream>

const char* ScoreManager::SCORE_FILE = "amazing_ball_highscore.dat";

ScoreManager::ScoreManager() : highScore(0) {
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
        highScore = 0;
        return false;
    }

    file >> highScore;
    file.close();

    // Validation du score chargé
    if (highScore < 0) {
        highScore = 0;
        return false;
    }

    return true;
}

void ScoreManager::saveHighScore(int score) {
    // Ne sauvegarder que si c'est un nouveau record
    if (score > highScore) {
        highScore = score;

        std::ofstream file(getScoreFilePath());

        if (!file.is_open()) {
            std::cerr << "Erreur: Impossible de sauvegarder le score dans " << getScoreFilePath() << std::endl;
            return;
        }

        file << highScore;
        file.close();
    }
}
