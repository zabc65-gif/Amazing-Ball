#ifndef ROOM_HPP
#define ROOM_HPP

#include <SDL2/SDL.h>
#include <vector>
#include "Vector2D.hpp"
#include "Menu.hpp"

struct Hole {
    Vector2D position;
    int radius;
};

struct Particle {
    Vector2D position;
    Vector2D velocity;
    int r, g, b;
    float life;
    float maxLife;
};

class Room {
public:
    Room(int screenWidth, int screenHeight, int level, Difficulty difficulty);
    ~Room();

    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void renderHUD(SDL_Renderer* renderer, int totalScore, float totalTime); // Afficher le score et le temps en premier plan
    bool isPlayerInHole(const Vector2D& playerPos, int playerRadius) const;
    bool hasReachedEnd(const Vector2D& playerPos) const;

    void startTimer();
    void stopTimer();
    int getScore() const;
    float getElapsedTime() const { return elapsedTime; }
    bool isCelebrating() const { return celebrating; }
    void createCelebrationParticles(const Vector2D& position);

    // Système de vies
    int getLives() const { return lives; }
    void loseLife();
    bool isGameOver() const { return lives <= 0; }

private:
    int screenWidth;
    int screenHeight;
    int level;
    Difficulty difficulty;

    std::vector<Hole> holes;
    std::vector<Particle> particles;
    int endZoneX;  // Zone à atteindre à droite
    int holeRadius;

    // Timer et score
    float elapsedTime;
    bool timerRunning;
    bool celebrating;
    float celebrationTime;

    // Système de vies
    int lives;
    bool gameOver;

    // Animation des flèches
    float arrowAnimPhase;

    void generateHoles();
    void drawArrow(SDL_Renderer* renderer, int x, int y, int size);
    void drawNumber(SDL_Renderer* renderer, int number, int x, int y, int size);
    void drawHeart(SDL_Renderer* renderer, int x, int y, int size, bool filled);
};

#endif
