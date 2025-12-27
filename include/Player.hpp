#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Vector2D.hpp"
#include <SDL2/SDL.h>
#include <deque>

enum class Direction {
    DOWN = 0,
    UP = 1,
    LEFT = 2,
    RIGHT = 3
};

class Player {
public:
    Player(float x, float y);
    ~Player();

    void handleInput();
    void handleEvent(SDL_Event& event);
    void update();
    void render(SDL_Renderer* renderer);

    Vector2D getPosition() const { return position; }
    bool isAttacking() const { return attacking; }
    Direction getDirection() const { return direction; }
    int getAttackRange() const { return 30; }
    int getRadius() const { return radius; }

private:
    void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
    void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
    void drawGradientCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius,
                           int r, int g, int b, int maxAlpha);
    Vector2D position;
    Vector2D velocity;
    Direction direction;

    float speed;
    int radius;

    // Système de saut
    bool isJumping;
    bool isGrounded;
    float jumpVelocity;
    float gravity;
    float verticalVelocity;
    float groundLevel;

    // Animation de pulsation du halo
    float haloPhase;
    float haloSpeed;

    // Satellite qui suit le joueur
    float satelliteOffsetX;
    float satelliteOffsetY;
    float satelliteFloatPhase;
    float satelliteFloatSpeed;
    int satelliteRadius;

    // Inertie du satellite
    float satelliteLagX;
    float satelliteLagY;
    Vector2D previousPosition;

    // Délai de réaction du satellite (historique de positions)
    std::deque<Vector2D> positionHistory;
    static const int SATELLITE_DELAY_FRAMES = 15;  // 0.25 seconde à 60 FPS

    bool attacking;
    int attackTimer;
    const int attackDuration = 15;
};

#endif
