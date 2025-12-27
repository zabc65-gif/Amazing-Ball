#include "Enemy.hpp"
#include "Room.hpp"
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
      health(2),
      maxHealth(2),
      lightRadius(80),
      detectionRadius(200.0f),
      attackRadius(30.0f),
      attackCooldown(0),
      attackCooldownMax(60),
      animationPhase(0.0f),
      animationSpeed(0.1f),
      knockbackVelocity(0, 0),
      knockbackFrames(0),
      knockbackDuration(15) {

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

bool Enemy::isHoleAt(float x, float y, Room* room) {
    if (!room) return false;
    // Utiliser isPlayerInHole avec un rayon très petit pour simuler un point
    Vector2D checkPos(x, y);
    return room->isPlayerInHole(checkPos, 1);
}

bool Enemy::isPathSafe(const Vector2D& target, Room* room) {
    if (!room) return true;

    // Vérifier plusieurs points le long du chemin
    Vector2D direction = target - position;
    float dist = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (dist < 1.0f) return true;

    direction.x /= dist;
    direction.y /= dist;

    // Vérifier des points tous les 10 pixels
    for (float d = 0; d < dist; d += 10.0f) {
        float checkX = position.x + direction.x * d;
        float checkY = position.y + direction.y * d;

        // Vérifier autour du rayon de l'ennemi
        if (isHoleAt(checkX, checkY, room) ||
            isHoleAt(checkX + radius, checkY, room) ||
            isHoleAt(checkX - radius, checkY, room) ||
            isHoleAt(checkX, checkY + radius, room) ||
            isHoleAt(checkX, checkY - radius, room)) {
            return false;
        }
    }

    return true;
}

void Enemy::updatePatrol(Room* room) {
    // Se déplacer vers la cible de patrouille
    float dist = distance(position, patrolTarget);

    if (dist < 5.0f || !isPathSafe(patrolTarget, room)) {
        // Arrivé à la cible ou chemin dangereux, en générer une nouvelle
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

        // Vérifier si la prochaine position est sûre
        Vector2D nextPos = position + velocity;
        if (isPathSafe(nextPos, room)) {
            position += velocity;
        } else {
            // Chemin bloqué, générer nouvelle cible
            generatePatrolTarget();
        }
    }
}

void Enemy::updateChase(const Vector2D& playerPos, Room* room) {
    // Se déplacer vers le joueur
    Vector2D direction = playerPos - position;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    }

    velocity = direction * speed;

    // Vérifier si la prochaine position est sûre
    Vector2D nextPos = position + velocity;
    if (isPathSafe(nextPos, room)) {
        position += velocity;
    } else {
        // Essayer de contourner par les côtés
        Vector2D sideDir1(-direction.y, direction.x); // Perpendiculaire à gauche
        Vector2D sideDir2(direction.y, -direction.x); // Perpendiculaire à droite

        Vector2D sidePos1 = position + sideDir1 * speed;
        Vector2D sidePos2 = position + sideDir2 * speed;

        if (isPathSafe(sidePos1, room)) {
            position = sidePos1;
        } else if (isPathSafe(sidePos2, room)) {
            position = sidePos2;
        }
        // Sinon rester sur place
    }
}

void Enemy::updateAttack() {
    // Réduire le cooldown d'attaque
    if (attackCooldown > 0) {
        attackCooldown--;
    }

    // L'ennemi s'arrête pendant l'attaque
    velocity.zero();
}

void Enemy::updateKnockback() {
    if (knockbackFrames > 0) {
        // Appliquer le recul
        position += knockbackVelocity;

        // Réduire progressivement la vélocité du recul
        knockbackVelocity.x *= 0.85f;
        knockbackVelocity.y *= 0.85f;

        knockbackFrames--;

        if (knockbackFrames == 0) {
            knockbackVelocity.zero();
        }
    }
}

void Enemy::update(const Vector2D& playerPos, Room* room) {
    if (state == EnemyState::DEAD) {
        return;
    }

    // Gérer le recul en priorité
    if (knockbackFrames > 0) {
        updateKnockback();
        return;
    }

    float distToPlayer = distance(position, playerPos);

    // Machine à états
    switch (state) {
        case EnemyState::PATROL:
            updatePatrol(room);

            // Détection du joueur
            if (distToPlayer < detectionRadius) {
                state = EnemyState::CHASE;
            }
            break;

        case EnemyState::CHASE:
            updateChase(playerPos, room);

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

void Enemy::takeDamage(int damage, const Vector2D& attackerPos) {
    if (state == EnemyState::DEAD) {
        return;
    }

    health -= damage;
    if (health <= 0) {
        health = 0;
        state = EnemyState::DEAD;
    } else {
        // Calculer la direction du recul (opposée à l'attaquant)
        Vector2D knockbackDir = position - attackerPos;
        float length = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);

        if (length > 0) {
            knockbackDir.x /= length;
            knockbackDir.y /= length;
        }

        // Recul de 2 fois la taille de l'ennemi
        float knockbackDistance = radius * 2.0f;
        knockbackVelocity = knockbackDir * (knockbackDistance / knockbackDuration);
        knockbackFrames = knockbackDuration;
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
