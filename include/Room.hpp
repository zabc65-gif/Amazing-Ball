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
    void renderHUD(SDL_Renderer* renderer, int totalScore, float totalTime, int playerHealth, bool gameOver); // Afficher le score et le temps en premier plan
    bool isPlayerInHole(const Vector2D& playerPos, int playerRadius) const;
    bool hasReachedEnd(const Vector2D& playerPos) const;
    bool isPlayerTouchingElectricStar(const Vector2D& playerPos, int playerRadius) const;
    Vector2D getElectricStarPos() const { return electricStarPos; }
    int getElectricStarRadius() const { return electricStarRadius; }
    Vector2D getSatelliteStarPos() const { return satelliteStarPos; }
    Difficulty getDifficulty() const { return difficulty; }

    void startTimer();
    void stopTimer();
    int getScore() const;
    float getElapsedTime() const { return elapsedTime; }
    bool isCelebrating() const { return celebrating; }
    void createCelebrationParticles(const Vector2D& position);

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

    // Animation des flèches
    float arrowAnimPhase;

    // Étoile électrique au centre
    Vector2D electricStarPos;
    int electricStarRadius;
    float electricAnimPhase;
    std::vector<float> boltAngles;  // Angles des éclairs

    // Étoile électrique satellite (mode difficile uniquement)
    Vector2D satelliteStarPos;
    float satelliteOrbitAngle;
    float satelliteOrbitSpeed;
    float satelliteOrbitRadius;

    void generateHoles();
    void drawElectricStar(SDL_Renderer* renderer);
    void drawArrow(SDL_Renderer* renderer, int x, int y, int size);
    void drawNumber(SDL_Renderer* renderer, int number, int x, int y, int size);
    void drawHeart(SDL_Renderer* renderer, int x, int y, int size, int quarters);
    void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, int size);
};

#endif
