#include "Enemy.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>

Enemy::Enemy(float x, float y)
    : position(x, y),
      velocity(0, 0),
      patrolTarget(x, y),
      state(EnemyState::PATROL),
      speed(1.5f),
      radius(12),
      health(3),
      maxHealth(3),
      detectionRadius(200.0f),
      attackRadius(30.0f),
      attackCooldown(0),
      attackCooldownMax(60),
      animationPhase(0.0f),
      animationSpeed(0.1f) {

    // Initialiser le générateur aléatoire
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }

    generatePatrolTarget();
}

Enemy::~Enemy() {}

void Enemy::generatePatrolTarget() {
    // Générer une cible de patrouille aléatoire dans les limites de l'écran
    float targetX = 80 + (std::rand() % (800 - 160));
    float targetY = 80 + (std::rand() % (600 - 160));
    patrolTarget = Vector2D(targetX, targetY);
}

float Enemy::distance(const Vector2D& a, const Vector2D& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}

void Enemy::updatePatrol() {
    // Se déplacer vers la cible de patrouille
    float dist = distance(position, patrolTarget);

    if (dist < 5.0f) {
        // Arrivé à la cible, en générer une nouvelle
        generatePatrolTarget();
    } else {
        // Se déplacer vers la cible
        Vector2D direction = patrolTarget - position;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0) {
            direction.x /= length;
            direction.y /= length;
        }

        velocity = direction * speed * 0.5f; // Patrouille plus lente
        position += velocity;
    }
}

void Enemy::updateChase(const Vector2D& playerPos) {
    // Se déplacer vers le joueur
    Vector2D direction = playerPos - position;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    }

    velocity = direction * speed;
    position += velocity;
}

void Enemy::updateAttack() {
    // Réduire le cooldown d'attaque
    if (attackCooldown > 0) {
        attackCooldown--;
    }

    // L'ennemi s'arrête pendant l'attaque
    velocity.zero();
}

void Enemy::update(const Vector2D& playerPos) {
    if (state == EnemyState::DEAD) {
        return;
    }

    float distToPlayer = distance(position, playerPos);

    // Machine à états
    switch (state) {
        case EnemyState::PATROL:
            updatePatrol();

            // Détection du joueur
            if (distToPlayer < detectionRadius) {
                state = EnemyState::CHASE;
            }
            break;

        case EnemyState::CHASE:
            updateChase(playerPos);

            // Le joueur est à portée d'attaque
            if (distToPlayer < attackRadius) {
                state = EnemyState::ATTACK;
                attackCooldown = attackCooldownMax;
            }

            // Le joueur s'est échappé
            if (distToPlayer > detectionRadius * 1.5f) {
                state = EnemyState::PATROL;
                generatePatrolTarget();
            }
            break;

        case EnemyState::ATTACK:
            updateAttack();

            // Retourner en poursuite si le joueur s'éloigne
            if (distToPlayer > attackRadius * 1.5f) {
                state = EnemyState::CHASE;
            }
            break;

        default:
            break;
    }

    // Limites de l'écran
    if (position.x < 40 + radius) position.x = 40 + radius;
    if (position.x > 800 - 40 - radius) position.x = 800 - 40 - radius;
    if (position.y < 40 + radius) position.y = 40 + radius;
    if (position.y > 600 - 40 - radius) position.y = 600 - 40 - radius;

    // Animation
    animationPhase += animationSpeed;
    if (animationPhase > 2 * M_PI) {
        animationPhase -= 2 * M_PI;
    }
}

void Enemy::takeDamage(int damage) {
    if (state == EnemyState::DEAD) {
        return;
    }

    health -= damage;
    if (health <= 0) {
        health = 0;
        state = EnemyState::DEAD;
    }
}

void Enemy::drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

void Enemy::drawGradientCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius,
                                int r, int g, int b, int maxAlpha) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            int distSquared = x * x + y * y;
            int radiusSquared = radius * radius;

            if (distSquared <= radiusSquared) {
                float dist = std::sqrt(static_cast<float>(distSquared)) / radius;
                float alpha = maxAlpha * (1.0f - dist * dist);

                SDL_SetRenderDrawColor(renderer, r, g, b, static_cast<Uint8>(alpha));
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

void Enemy::render(SDL_Renderer* renderer) {
    if (state == EnemyState::DEAD) {
        return;
    }

    int centerX = static_cast<int>(position.x);
    int centerY = static_cast<int>(position.y);

    // Ombre
    int shadowY = centerY + 15;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 40);
    drawFilledCircle(renderer, centerX, shadowY, radius);

    // Pulsation basée sur l'état
    float pulse = 1.0f;
    if (state == EnemyState::CHASE) {
        pulse = 1.0f + 0.2f * std::sin(animationPhase * 3);
    } else if (state == EnemyState::ATTACK) {
        pulse = 1.0f + 0.3f * std::sin(animationPhase * 5);
    } else {
        pulse = 1.0f + 0.1f * std::sin(animationPhase);
    }

    int currentRadius = static_cast<int>(radius * pulse);

    // Halo (rouge pour ennemi)
    int haloRadius = static_cast<int>(currentRadius * 1.8f);
    drawGradientCircle(renderer, centerX, centerY, haloRadius, 255, 0, 0, 60);

    // Corps de l'ennemi - rouge sombre
    SDL_SetRenderDrawColor(renderer, 150, 30, 30, 255);
    drawFilledCircle(renderer, centerX, centerY, currentRadius);

    // Centre lumineux rouge
    SDL_SetRenderDrawColor(renderer, 255, 60, 60, 255);
    drawFilledCircle(renderer, centerX, centerY, currentRadius / 2);

    // Brillance
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    drawFilledCircle(renderer, centerX - 2, centerY - 2, currentRadius / 3);

    // Barre de vie
    int barWidth = 30;
    int barHeight = 4;
    int barX = centerX - barWidth / 2;
    int barY = centerY - radius - 10;

    // Fond de la barre
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_Rect bgRect = {barX, barY, barWidth, barHeight};
    SDL_RenderFillRect(renderer, &bgRect);

    // Vie actuelle
    int healthWidth = (health * barWidth) / maxHealth;
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    SDL_Rect healthRect = {barX, barY, healthWidth, barHeight};
    SDL_RenderFillRect(renderer, &healthRect);
}
