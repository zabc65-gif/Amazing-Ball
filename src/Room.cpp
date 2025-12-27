#include "Room.hpp"
#include "Player.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <algorithm>

Room::Room(int screenWidth, int screenHeight, int level, Difficulty difficulty)
    : screenWidth(screenWidth),
      screenHeight(screenHeight),
      level(level),
      difficulty(difficulty),
      endZoneX(screenWidth - 100),
      elapsedTime(0.0f),
      timerRunning(false),
      celebrating(false),
      celebrationTime(0.0f),
      lives(3),
      gameOver(false),
      arrowAnimPhase(0.0f) {

    // Le rayon d'un trou est le double de la taille du joueur (rayon du joueur = 8)
    holeRadius = 16;

    // Initialiser le générateur de nombres aléatoires
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int>(time(nullptr)));
        seeded = true;
    }

    generateHoles();
}

Room::~Room() {}

void Room::generateHoles() {
    holes.clear();

    // Nombre de trous = 3 * niveau * difficulté
    int numHoles = 3 * level * static_cast<int>(difficulty);

    // Zone de jeu pour les trous (éviter les bords et les zones de départ/arrivée)
    int startZone = 150;  // Zone de départ à gauche
    int endZone = 150;    // Zone d'arrivée à droite
    int topMargin = 100;
    int bottomMargin = 100;

    int minX = startZone;
    int maxX = screenWidth - endZone;
    int minY = topMargin;
    int maxY = screenHeight - bottomMargin;

    // Générer les trous aléatoirement
    for (int i = 0; i < numHoles; i++) {
        bool validPosition = false;
        Vector2D newPos;
        int attempts = 0;
        const int maxAttempts = 100;

        // Trouver une position valide (pas trop proche des autres trous)
        while (!validPosition && attempts < maxAttempts) {
            newPos.x = static_cast<float>(minX + rand() % (maxX - minX));
            newPos.y = static_cast<float>(minY + rand() % (maxY - minY));

            validPosition = true;

            // Vérifier la distance avec les autres trous
            for (const auto& hole : holes) {
                float dx = newPos.x - hole.position.x;
                float dy = newPos.y - hole.position.y;
                float distance = std::sqrt(dx * dx + dy * dy);

                // Distance minimale entre les trous = 3 * rayon du trou
                if (distance < holeRadius * 3) {
                    validPosition = false;
                    break;
                }
            }

            attempts++;
        }

        if (validPosition) {
            Hole hole;
            hole.position = newPos;
            hole.radius = holeRadius;
            holes.push_back(hole);
        }
    }
}

void Room::startTimer() {
    timerRunning = true;
    elapsedTime = 0.0f;
}

void Room::stopTimer() {
    timerRunning = false;
}

int Room::getScore() const {
    // Score basé sur le temps: max 1000 points, diminue avec le temps
    // Formule: 1000 - (temps * 10)
    int score = 1000 - static_cast<int>(elapsedTime * 10);
    if (score < 0) score = 0;
    return score;
}

void Room::loseLife() {
    if (lives > 0) {
        lives--;
        if (lives <= 0) {
            gameOver = true;
            stopTimer();
        }
    }
}

void Room::update(float deltaTime) {
    // Mettre à jour le timer
    if (timerRunning) {
        elapsedTime += deltaTime;
    }

    // Mettre à jour l'animation des flèches
    arrowAnimPhase += deltaTime * 3.0f;
    if (arrowAnimPhase > 2 * M_PI) {
        arrowAnimPhase -= 2 * M_PI;
    }

    // Mettre à jour la célébration
    if (celebrating) {
        celebrationTime += deltaTime;

        // Mettre à jour les particules
        for (auto& particle : particles) {
            particle.position += particle.velocity * deltaTime;
            particle.velocity.y += 200.0f * deltaTime; // Gravité
            particle.life -= deltaTime;
        }

        // Supprimer les particules mortes
        particles.erase(
            std::remove_if(particles.begin(), particles.end(),
                [](const Particle& p) { return p.life <= 0.0f; }),
            particles.end()
        );
    }
}

void Room::drawArrow(SDL_Renderer* renderer, int x, int y, int size) {
    // Dessiner une flèche pointant vers la droite avec une pointe bien visible

    // Corps de la flèche (rectangle)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Jaune vif
    SDL_Rect arrowBody = {x - size/2, y - size/6, size, size/3};
    SDL_RenderFillRect(renderer, &arrowBody);

    // Pointe de la flèche (triangle)
    for (int i = 0; i < size/2; i++) {
        int height = size - i * 2;
        for (int j = 0; j < height; j++) {
            SDL_RenderDrawPoint(renderer, x + size/2 + i, y - height/2 + j);
        }
    }

    // Contour noir pour mieux voir la flèche
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Contour du corps
    SDL_Rect outlineBody = {x - size/2 - 1, y - size/6 - 1, size + 2, size/3 + 2};
    SDL_RenderDrawRect(renderer, &outlineBody);

    // Contour de la pointe (lignes)
    // Ligne supérieure
    for (int i = 0; i <= size/2; i++) {
        SDL_RenderDrawPoint(renderer, x + size/2 + i, y - (size - i * 2)/2 - 1);
    }
    // Ligne inférieure
    for (int i = 0; i <= size/2; i++) {
        SDL_RenderDrawPoint(renderer, x + size/2 + i, y + (size - i * 2)/2 + 1);
    }
}

void Room::createCelebrationParticles(const Vector2D& position) {
    celebrating = true;
    celebrationTime = 0.0f;

    // Créer des particules colorées
    for (int i = 0; i < 50; i++) {
        Particle p;
        p.position = position;

        // Vitesse aléatoire dans toutes les directions (vers le haut principalement)
        float angle = (rand() % 360) * M_PI / 180.0f;
        float speed = 100.0f + (rand() % 200);
        p.velocity.x = cos(angle) * speed;
        p.velocity.y = -abs(sin(angle) * speed); // Vers le haut

        // Couleurs aléatoires vives
        int colorChoice = rand() % 5;
        switch (colorChoice) {
            case 0: p.r = 255; p.g = 0; p.b = 0; break;     // Rouge
            case 1: p.r = 255; p.g = 255; p.b = 0; break;   // Jaune
            case 2: p.r = 0; p.g = 255; p.b = 0; break;     // Vert
            case 3: p.r = 0; p.g = 255; p.b = 255; break;   // Cyan
            case 4: p.r = 255; p.g = 0; p.b = 255; break;   // Magenta
        }

        p.life = 1.0f + (rand() % 100) / 100.0f;
        p.maxLife = p.life;

        particles.push_back(p);
    }
}

void Room::drawNumber(SDL_Renderer* renderer, int number, int x, int y, int size) {
    // Convertir le nombre en string
    std::string numStr = std::to_string(number);

    int currentX = x;
    int digitWidth = size * 6;
    int digitSpacing = size * 2;

    for (char digit : numStr) {
        int d = digit - '0';

        // Dessiner chaque chiffre pixel par pixel
        for (int py = 0; py < 7; py++) {
            for (int px = 0; px < 5; px++) {
                bool drawPixel = false;

                // Patterns pour chaque chiffre
                switch (d) {
                    case 0:
                        drawPixel = (py == 0 && px > 0 && px < 4) ||
                                   (py == 6 && px > 0 && px < 4) ||
                                   ((px == 0 || px == 4) && py > 0 && py < 6);
                        break;
                    case 1:
                        drawPixel = (px == 2) || (py == 6);
                        break;
                    case 2:
                        drawPixel = (py == 0) || (py == 3) || (py == 6) ||
                                   (px == 4 && py > 0 && py < 3) ||
                                   (px == 0 && py > 3 && py < 6);
                        break;
                    case 3:
                        drawPixel = (py == 0) || (py == 3) || (py == 6) ||
                                   (px == 4 && py > 0 && py < 6);
                        break;
                    case 4:
                        drawPixel = (px == 4) || (py == 3) ||
                                   (px == 0 && py < 3);
                        break;
                    case 5:
                        drawPixel = (py == 0) || (py == 3) || (py == 6) ||
                                   (px == 0 && py > 0 && py < 3) ||
                                   (px == 4 && py > 3 && py < 6);
                        break;
                    case 6:
                        drawPixel = (py == 0 && px > 0) || (py == 3) || (py == 6 && px > 0 && px < 4) ||
                                   (px == 0 && py > 0) ||
                                   (px == 4 && py > 3 && py < 6);
                        break;
                    case 7:
                        drawPixel = (py == 0) || (px == 4 && py > 0);
                        break;
                    case 8:
                        drawPixel = (py == 0) || (py == 3) || (py == 6) ||
                                   (px == 0 && py > 0 && py < 6) ||
                                   (px == 4 && py > 0 && py < 6);
                        break;
                    case 9:
                        drawPixel = (py == 0) || (py == 3) || (py == 6 && px < 4) ||
                                   (px == 0 && py > 0 && py < 3) ||
                                   (px == 4);
                        break;
                }

                if (drawPixel) {
                    for (int sy = 0; sy < size; sy++) {
                        for (int sx = 0; sx < size; sx++) {
                            SDL_RenderDrawPoint(renderer,
                                              currentX + px * size + sx,
                                              y + py * size + sy);
                        }
                    }
                }
            }
        }

        currentX += digitWidth + digitSpacing;
    }
}

void Room::drawHeart(SDL_Renderer* renderer, int x, int y, int size, bool filled) {
    // Dessiner un cœur pixel art
    // Pattern du cœur (8x8)
    bool heartPattern[8][8] = {
        {0,1,1,0,0,1,1,0},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,0,0},
        {0,0,0,1,1,0,0,0},
        {0,0,0,0,0,0,0,0}
    };

    for (int py = 0; py < 8; py++) {
        for (int px = 0; px < 8; px++) {
            if (heartPattern[py][px]) {
                if (filled) {
                    // Cœur plein (rouge)
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                } else {
                    // Cœur vide (contour gris)
                    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                }

                for (int sy = 0; sy < size; sy++) {
                    for (int sx = 0; sx < size; sx++) {
                        SDL_RenderDrawPoint(renderer,
                                          x + px * size + sx,
                                          y + py * size + sy);
                    }
                }
            }
        }
    }
}

void Room::render(SDL_Renderer* renderer) {
    // Fond de la salle (couleur légèrement différente)
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderClear(renderer);

    // Dessiner la zone de départ (gauche) en vert
    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
    SDL_Rect startZone = {0, 0, 100, screenHeight};
    SDL_RenderFillRect(renderer, &startZone);

    // Dessiner les flèches dans la zone de départ
    int arrowSize = 20;
    float animOffset = sin(arrowAnimPhase) * 10.0f;

    // Dessiner 3 flèches animées
    for (int i = 0; i < 3; i++) {
        int arrowY = screenHeight / 4 + i * screenHeight / 4;
        drawArrow(renderer, 30 + static_cast<int>(animOffset), arrowY - arrowSize/2, arrowSize);
    }

    // Dessiner la zone d'arrivée (droite) avec des lignes verticales (damier)
    SDL_SetRenderDrawColor(renderer, 0, 0, 150, 255);
    SDL_Rect endZone = {screenWidth - 100, 0, 100, screenHeight};
    SDL_RenderFillRect(renderer, &endZone);

    // Lignes blanches de la ligne d'arrivée
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < screenHeight; y += 40) {
        for (int x = 0; x < 100; x += 20) {
            SDL_Rect checkRect = {screenWidth - 100 + x, y, 20, 20};
            if ((x / 20 + y / 40) % 2 == 0) {
                SDL_RenderFillRect(renderer, &checkRect);
            }
        }
    }

    // Dessiner les trous (noir avec bordure rouge)
    for (const auto& hole : holes) {
        // Dessiner le trou (cercle noir)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // Dessiner un cercle rempli pour le trou
        for (int w = 0; w < hole.radius * 2; w++) {
            for (int h = 0; h < hole.radius * 2; h++) {
                int dx = hole.radius - w;
                int dy = hole.radius - h;
                if ((dx*dx + dy*dy) <= (hole.radius * hole.radius)) {
                    SDL_RenderDrawPoint(renderer,
                                      static_cast<int>(hole.position.x) + dx,
                                      static_cast<int>(hole.position.y) + dy);
                }
            }
        }

        // Bordure rouge autour du trou
        SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        int outerRadius = hole.radius + 2;
        for (int w = 0; w < outerRadius * 2; w++) {
            for (int h = 0; h < outerRadius * 2; h++) {
                int dx = outerRadius - w;
                int dy = outerRadius - h;
                int distSq = dx*dx + dy*dy;
                if (distSq <= (outerRadius * outerRadius) &&
                    distSq >= (hole.radius * hole.radius)) {
                    SDL_RenderDrawPoint(renderer,
                                      static_cast<int>(hole.position.x) + dx,
                                      static_cast<int>(hole.position.y) + dy);
                }
            }
        }
    }

    // Dessiner les particules de célébration
    for (const auto& particle : particles) {
        float alpha = particle.life / particle.maxLife;
        SDL_SetRenderDrawColor(renderer,
                              particle.r,
                              particle.g,
                              particle.b,
                              static_cast<Uint8>(255 * alpha));

        // Dessiner une petite étoile pour chaque particule
        int px = static_cast<int>(particle.position.x);
        int py = static_cast<int>(particle.position.y);

        // Croix de 5 pixels
        for (int i = -2; i <= 2; i++) {
            SDL_RenderDrawPoint(renderer, px + i, py);
            SDL_RenderDrawPoint(renderer, px, py + i);
        }
        // Diagonales
        SDL_RenderDrawPoint(renderer, px + 1, py + 1);
        SDL_RenderDrawPoint(renderer, px - 1, py - 1);
        SDL_RenderDrawPoint(renderer, px + 1, py - 1);
        SDL_RenderDrawPoint(renderer, px - 1, py + 1);
    }
}

void Room::renderHUD(SDL_Renderer* renderer, int totalScore) {
    // Afficher les cœurs en haut à gauche
    int heartSize = 2;
    int heartSpacing = heartSize * 10;
    for (int i = 0; i < 3; i++) {
        drawHeart(renderer, 20 + i * heartSpacing, 20, heartSize, i < lives);
    }

    // Afficher le temps en haut à droite
    if (timerRunning || celebrating || gameOver) {
        int timeInSeconds = static_cast<int>(elapsedTime);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawNumber(renderer, timeInSeconds, screenWidth - 100, 20, 2);

        // Ajouter "s" pour secondes (dessin simple)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int sX = screenWidth - 40;
        int sY = 20;
        // Dessiner un "S" simplifié
        for (int py = 0; py < 7; py++) {
            for (int px = 0; px < 5; px++) {
                bool drawPixel = (py == 0 && px > 0) ||
                               (px == 0 && py > 0 && py < 3) ||
                               (py == 3 && px > 0 && px < 4) ||
                               (px == 4 && py > 3 && py < 6) ||
                               (py == 6 && px < 4);
                if (drawPixel) {
                    for (int sy = 0; sy < 2; sy++) {
                        for (int sx = 0; sx < 2; sx++) {
                            SDL_RenderDrawPoint(renderer, sX + px * 2 + sx, sY + py * 2 + sy);
                        }
                    }
                }
            }
        }
    }

    // Afficher le score total + score du niveau actuel en bas à droite
    if (timerRunning || celebrating || gameOver) {
        int currentLevelScore = getScore();
        int displayScore = totalScore + currentLevelScore;
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Jaune pour le score
        drawNumber(renderer, displayScore, screenWidth - 150, screenHeight - 50, 3);
    }

    // Écran de game over
    if (gameOver) {
        // Fond semi-transparent
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_Rect overlay = {0, 0, screenWidth, screenHeight};
        SDL_RenderFillRect(renderer, &overlay);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        // Texte "GAME OVER" centré (simplifié avec des rectangles)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        // On pourrait utiliser drawText ici mais pour simplifier on affiche juste le message

        // Afficher le temps et le score total final au centre
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int centerY = screenHeight / 2;
        drawNumber(renderer, static_cast<int>(elapsedTime), screenWidth / 2 - 50, centerY, 4);

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        int finalScore = totalScore + getScore();
        drawNumber(renderer, finalScore, screenWidth / 2 - 80, centerY + 80, 4);
    }
}

bool Room::isPlayerInHole(const Vector2D& playerPos, int playerRadius) const {
    for (const auto& hole : holes) {
        float dx = playerPos.x - hole.position.x;
        float dy = playerPos.y - hole.position.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // Si le centre du joueur est dans le trou
        if (distance <= hole.radius - playerRadius / 2) {
            return true;
        }
    }
    return false;
}

bool Room::hasReachedEnd(const Vector2D& playerPos) const {
    // Le joueur a atteint la fin s'il est dans la zone de droite
    return playerPos.x >= endZoneX;
}
