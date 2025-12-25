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

class Enemy {
public:
    Enemy(float x, float y);
    ~Enemy();

    void update(const Vector2D& playerPos);
    void render(SDL_Renderer* renderer);

    Vector2D getPosition() const { return position; }
    int getRadius() const { return radius; }
    bool isDead() const { return state == EnemyState::DEAD; }

    void takeDamage(int damage);

private:
    Vector2D position;
    Vector2D velocity;
    Vector2D patrolTarget;

    EnemyState state;

    float speed;
    int radius;
    int health;
    int maxHealth;

    float detectionRadius;
    float attackRadius;

    int attackCooldown;
    int attackCooldownMax;

    // Animation
    float animationPhase;
    float animationSpeed;

    // Helpers
    void updatePatrol();
    void updateChase(const Vector2D& playerPos);
    void updateAttack();

    void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
    void drawGradientCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius,
                           int r, int g, int b, int maxAlpha);

    float distance(const Vector2D& a, const Vector2D& b);
    void generatePatrolTarget();
};

#endif
