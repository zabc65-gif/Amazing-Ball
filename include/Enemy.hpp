#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Vector2D.hpp"
#include <SDL2/SDL.h>

enum class EnemyState {
    IDLE,
    PATROL,
    CHASE,
    ATTACK,
    DEAD
};

class Room;

class Enemy {
public:
    Enemy(float x, float y);
    ~Enemy();

    void update(const Vector2D& playerPos, Room* room);
    void render(SDL_Renderer* renderer);

    Vector2D getPosition() const { return position; }
    int getRadius() const { return radius; }
    int getLightRadius() const { return lightRadius; }
    bool isDead() const { return state == EnemyState::DEAD; }

    void takeDamage(int damage, const Vector2D& attackerPos);

private:
    Vector2D position;
    Vector2D velocity;
    Vector2D patrolTarget;

    EnemyState state;

    float speed;
    int radius;
    int health;
    int maxHealth;
    int lightRadius;

    float detectionRadius;
    float attackRadius;

    int attackCooldown;
    int attackCooldownMax;

    // Animation
    float animationPhase;
    float animationSpeed;

    // Recul
    Vector2D knockbackVelocity;
    int knockbackFrames;
    int knockbackDuration;

    // Helpers
    void updatePatrol(Room* room);
    void updateChase(const Vector2D& playerPos, Room* room);
    void updateAttack();
    void updateKnockback();

    void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
    void drawGradientCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius,
                           int r, int g, int b, int maxAlpha);

    float distance(const Vector2D& a, const Vector2D& b);
    void generatePatrolTarget();
    bool isHoleAt(float x, float y, Room* room);
    bool isPathSafe(const Vector2D& target, Room* room);
    bool isInForbiddenZone(float x, float y) const;
};

#endif
