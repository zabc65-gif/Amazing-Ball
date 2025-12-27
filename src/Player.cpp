#include "Player.hpp"
#include <cmath>

Player::Player(float x, float y)
    : position(x, y),
      velocity(0, 0),
      direction(Direction::DOWN),
      speed(3.5f),
      radius(8),  // Réduit de 30% supplémentaires (12 -> 8.4, arrondi à 8)
      isJumping(false),
      isGrounded(true),
      jumpVelocity(-7.5f),   // Vitesse initiale du saut (ajustée)
      gravity(0.6f),          // Force de gravité
      verticalVelocity(0.0f),
      groundLevel(y),
      haloPhase(0.0f),
      haloSpeed(0.08f),
      satelliteOffsetX(25.0f),  // À droite du joueur
      satelliteOffsetY(-20.0f), // Au-dessus du joueur
      satelliteFloatPhase(0.0f),
      satelliteFloatSpeed(0.05f),
      satelliteRadius(6),
      satelliteLagX(0.0f),
      satelliteLagY(0.0f),
      previousPosition(x, y),
      attacking(false),
      attackTimer(0) {
    // Initialiser l'historique de positions avec la position de départ
    for (int i = 0; i < SATELLITE_DELAY_FRAMES; i++) {
        positionHistory.push_back(Vector2D(x, y));
    }
}

Player::~Player() {}

void Player::handleInput() {
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);

    velocity.zero();

    // Déplacement avec les flèches
    if (keyState[SDL_SCANCODE_UP]) {
        velocity.y = -speed;
        direction = Direction::UP;
    }
    if (keyState[SDL_SCANCODE_DOWN]) {
        velocity.y = speed;
        direction = Direction::DOWN;
    }
    if (keyState[SDL_SCANCODE_LEFT]) {
        velocity.x = -speed;
        direction = Direction::LEFT;
    }
    if (keyState[SDL_SCANCODE_RIGHT]) {
        velocity.x = speed;
        direction = Direction::RIGHT;
    }
}

void Player::handleEvent(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        // Saut avec W ou Espace
        if ((event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_SPACE) && isGrounded && !attacking) {
            isJumping = true;
            isGrounded = false;
            verticalVelocity = jumpVelocity;
        }

        // Attaque avec Shift
        if (event.key.keysym.sym == SDLK_LSHIFT && !attacking) {
            attacking = true;
            attackTimer = attackDuration;
        }
    }
}

void Player::update() {
    // Mise à jour de la position horizontale
    position += velocity;

    // Physique du saut
    if (!isGrounded) {
        // Appliquer la gravité
        verticalVelocity += gravity;
        position.y += verticalVelocity;

        // Vérifier si on touche le sol
        if (position.y >= groundLevel) {
            position.y = groundLevel;
            verticalVelocity = 0.0f;
            isGrounded = true;
            isJumping = false;
        }
    }

    // Limites de l'écran (on suppose une fenêtre 800x600)
    if (position.x < 40 + radius) position.x = 40 + radius;
    if (position.x > 800 - 40 - radius) position.x = 800 - 40 - radius;

    // Limites verticales uniquement pour empêcher de sortir par le haut
    if (position.y < 40 + radius) {
        position.y = 40 + radius;
        verticalVelocity = 0.0f;
    }

    // Mettre à jour l'historique des positions
    positionHistory.push_back(position);
    if (positionHistory.size() > SATELLITE_DELAY_FRAMES) {
        positionHistory.pop_front();
    }

    // Utiliser la position d'il y a 15 frames pour le satellite
    // Sécurité: utiliser la position actuelle si l'historique n'est pas encore complet
    Vector2D delayedPosition = positionHistory.empty() ? position : positionHistory.front();

    // Calculer le déplacement basé sur la position retardée
    Vector2D movement = delayedPosition - previousPosition;

    // Mise à jour de l'inertie du satellite avec un effet de ressort
    // Le satellite "traîne" derrière le joueur quand il se déplace
    const float lagFactor = 0.08f;  // Force du décalage (plus petit = plus d'inertie)
    const float maxLag = 20.0f;     // Décalage maximum en pixels

    // Calculer le décalage basé sur le mouvement
    float targetLagX = -movement.x * 5.0f;  // Inverse du mouvement (augmenté)
    float targetLagY = -movement.y * 5.0f;

    // Limiter le décalage maximum
    if (targetLagX > maxLag) targetLagX = maxLag;
    if (targetLagX < -maxLag) targetLagX = -maxLag;
    if (targetLagY > maxLag) targetLagY = maxLag;
    if (targetLagY < -maxLag) targetLagY = -maxLag;

    // Interpolation douce vers la position cible (effet ressort)
    satelliteLagX += (targetLagX - satelliteLagX) * lagFactor;
    satelliteLagY += (targetLagY - satelliteLagY) * lagFactor;

    // Retour progressif à 0 quand le joueur ne bouge plus
    if (std::abs(movement.x) < 0.1f && std::abs(movement.y) < 0.1f) {
        satelliteLagX *= 0.85f;  // Retour plus lent à la position d'origine
        satelliteLagY *= 0.85f;
    }

    // Sauvegarder la position retardée pour la prochaine frame
    previousPosition = delayedPosition;

    // Animation de pulsation du halo
    haloPhase += haloSpeed;
    if (haloPhase > 2 * M_PI) {
        haloPhase -= 2 * M_PI;
    }

    // Animation de flottement du satellite
    satelliteFloatPhase += satelliteFloatSpeed;
    if (satelliteFloatPhase > 2 * M_PI) {
        satelliteFloatPhase -= 2 * M_PI;
    }

    // Mise à jour de l'attaque
    if (attacking) {
        attackTimer--;
        if (attackTimer <= 0) {
            attacking = false;
        }
    }
}

void Player::drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    // Algorithme de Bresenham pour dessiner un cercle
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void Player::drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    // Dessiner un cercle rempli
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

void Player::drawGradientCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius,
                                int r, int g, int b, int maxAlpha) {
    // Dessiner un cercle avec dégradé de transparence
    // La transparence diminue du centre vers les bords
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            int distSquared = x * x + y * y;
            int radiusSquared = radius * radius;

            if (distSquared <= radiusSquared) {
                // Calculer la distance normalisée (0 au centre, 1 au bord)
                float dist = std::sqrt(static_cast<float>(distSquared)) / radius;

                // Inverser pour avoir maxAlpha au centre et 0 au bord
                // Utiliser une courbe douce pour la transition
                float alpha = maxAlpha * (1.0f - dist * dist);

                SDL_SetRenderDrawColor(renderer, r, g, b, static_cast<Uint8>(alpha));
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

void Player::render(SDL_Renderer* renderer) {
    int centerX = static_cast<int>(position.x);
    int centerY = static_cast<int>(position.y);

    // ===== SATELLITE =====
    // Utiliser la position retardée pour le satellite
    // Sécurité: utiliser la position actuelle si l'historique n'est pas encore complet
    Vector2D delayedPosition = positionHistory.empty() ? position : positionHistory.front();

    // Calculer la position du satellite avec effet de flottement et inertie
    float floatOffset = 4.0f * std::sin(satelliteFloatPhase);
    int satX = static_cast<int>(delayedPosition.x + satelliteOffsetX + satelliteLagX);
    int satY = static_cast<int>(delayedPosition.y + satelliteOffsetY + floatOffset + satelliteLagY);

    // Ombre du satellite pour donner l'impression de profondeur
    int shadowY = static_cast<int>(delayedPosition.y + satelliteOffsetY + satelliteLagY + 15);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 30);
    drawFilledCircle(renderer, satX, shadowY, satelliteRadius - 1);

    // Halo orange du satellite (plus petit)
    drawGradientCircle(renderer, satX, satY, satelliteRadius * 2, 255, 140, 0, 40);

    // Corps du satellite - orange foncé
    SDL_SetRenderDrawColor(renderer, 200, 80, 0, 255);
    drawFilledCircle(renderer, satX, satY, satelliteRadius);

    // Centre lumineux orange
    SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255);
    drawFilledCircle(renderer, satX, satY, satelliteRadius / 2);

    // Petite brillance blanche
    SDL_SetRenderDrawColor(renderer, 255, 200, 100, 255);
    drawFilledCircle(renderer, satX - 1, satY - 1, satelliteRadius / 3);

    // ===== JOUEUR =====
    // Ombre du joueur - dessinée en premier pour être derrière le halo
    int playerShadowY = centerY + 25;  // Bien en dessous pour être visible
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 60);
    drawFilledCircle(renderer, centerX, playerShadowY, radius + 2);

    // Calculer la pulsation du halo (oscille entre 0.9 et 1.1)
    float haloPulse = 1.0f + 0.1f * std::sin(haloPhase);

    // Dessiner les halos avec dégradé (du plus grand au plus petit)
    // Halo externe 1 - très subtil, s'estompe complètement aux bords
    int haloRadius1 = static_cast<int>(radius * 3.0f * haloPulse);
    drawGradientCircle(renderer, centerX, centerY, haloRadius1, 255, 255, 255, 30);

    // Halo externe 2 - dégradé doux
    int haloRadius2 = static_cast<int>(radius * 2.2f * haloPulse);
    drawGradientCircle(renderer, centerX, centerY, haloRadius2, 255, 255, 255, 50);

    // Halo interne - plus intense
    int haloRadius3 = static_cast<int>(radius * 1.5f);
    drawGradientCircle(renderer, centerX, centerY, haloRadius3, 255, 255, 255, 100);

    // Bord lumineux de la boule
    int glowRadius = static_cast<int>(radius * 1.2f);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 180);
    drawFilledCircle(renderer, centerX, centerY, glowRadius);

    // Coeur blanc brillant de la boule
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    drawFilledCircle(renderer, centerX, centerY, radius);

    // Centre ultra-lumineux (petit cercle au centre)
    int coreRadius = static_cast<int>(radius * 0.5f);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    drawFilledCircle(renderer, centerX, centerY, coreRadius);

    // Si le joueur attaque, dessiner un éclair d'énergie
    if (attacking) {
        float attackIntensity = static_cast<float>(attackTimer) / attackDuration;

        // Éclair d'énergie qui suit la direction
        int energyLength = 30;
        int energyWidth = 6;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, static_cast<Uint8>(200 * attackIntensity));

        switch (direction) {
            case Direction::UP:
                for (int i = 0; i < energyLength; i++) {
                    int y = centerY - radius - i;
                    int thickness = energyWidth - (i * energyWidth / energyLength);
                    for (int dx = -thickness; dx <= thickness; dx++) {
                        SDL_RenderDrawPoint(renderer, centerX + dx, y);
                    }
                }
                break;
            case Direction::DOWN:
                for (int i = 0; i < energyLength; i++) {
                    int y = centerY + radius + i;
                    int thickness = energyWidth - (i * energyWidth / energyLength);
                    for (int dx = -thickness; dx <= thickness; dx++) {
                        SDL_RenderDrawPoint(renderer, centerX + dx, y);
                    }
                }
                break;
            case Direction::LEFT:
                for (int i = 0; i < energyLength; i++) {
                    int x = centerX - radius - i;
                    int thickness = energyWidth - (i * energyWidth / energyLength);
                    for (int dy = -thickness; dy <= thickness; dy++) {
                        SDL_RenderDrawPoint(renderer, x, centerY + dy);
                    }
                }
                break;
            case Direction::RIGHT:
                for (int i = 0; i < energyLength; i++) {
                    int x = centerX + radius + i;
                    int thickness = energyWidth - (i * energyWidth / energyLength);
                    for (int dy = -thickness; dy <= thickness; dy++) {
                        SDL_RenderDrawPoint(renderer, x, centerY + dy);
                    }
                }
                break;
        }
    }
}
