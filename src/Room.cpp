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

void Room::drawHeart(SDL_Renderer* renderer, int x, int y, int size, int quarters) {
    // Dessiner un cœur pixel art avec support des quarts
    // quarters: 0 = vide, 1 = 1/4, 2 = 2/4, 3 = 3/4, 4 = plein

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
                // Déterminer si ce pixel doit être rempli selon le niveau de quarts
                bool shouldFill = false;

                if (quarters == 4) {
                    // Cœur complètement plein
                    shouldFill = true;
                } else if (quarters == 3) {
                    // 3/4 plein - remplir tout sauf le quart supérieur droit
                    shouldFill = !(px >= 4 && py < 2);
                } else if (quarters == 2) {
                    // 2/4 plein - remplir la moitié inférieure
                    shouldFill = (py >= 4);
                } else if (quarters == 1) {
                    // 1/4 plein - remplir le quart inférieur
                    shouldFill = (py >= 6);
                }

                if (shouldFill) {
                    // Partie remplie (rouge)
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                } else {
                    // Partie vide ou contour (gris)
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

    // Afficher le texte d'instruction en haut de l'écran (en premier plan)
    std::string instruction = "Rejoins l'autre cote";
    int textSize = 2;
    int charWidth = 8 * textSize;
    int spacing = 2 * textSize;
    int textWidth = instruction.length() * (charWidth + spacing);
    int textX = (screenWidth - textWidth) / 2;
    int textY = 15;

    SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);  // Jaune clair
    drawText(renderer, instruction, textX, textY, textSize);
}

void Room::renderHUD(SDL_Renderer* renderer, int totalScore, float totalTime, int playerHealth, bool gameOver) {
    // Afficher les cœurs en haut à gauche
    // playerHealth est en quarts de cœur (12 = 3 cœurs pleins, 11 = 2 cœurs pleins + 3/4, etc.)
    int heartSize = 2;
    int heartSpacing = heartSize * 10;

    for (int i = 0; i < 3; i++) {
        // Calculer le nombre de quarts pour ce cœur
        int quartersForThisHeart = playerHealth - (i * 4);

        // Limiter entre 0 et 4
        if (quartersForThisHeart > 4) {
            quartersForThisHeart = 4;
        } else if (quartersForThisHeart < 0) {
            quartersForThisHeart = 0;
        }

        drawHeart(renderer, 20 + i * heartSpacing, 20, heartSize, quartersForThisHeart);
    }

    // Afficher le temps total + temps du niveau actuel en haut à droite
    if (timerRunning || celebrating || gameOver) {
        int displayTime = static_cast<int>(totalTime + elapsedTime);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawNumber(renderer, displayTime, screenWidth - 100, 20, 2);

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

    // Afficher le score total + score du niveau actuel dans une fenêtre en surbrillance à gauche
    if (timerRunning || celebrating || gameOver) {
        int currentLevelScore = getScore();
        int displayScore = totalScore + currentLevelScore;

        // Position de la fenêtre de score
        int boxX = 15;
        int boxY = screenHeight - 80;
        int boxWidth = 150;
        int boxHeight = 60;

        // Fond semi-transparent avec bordure dorée
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Bordure extérieure dorée (effet de surbrillance)
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 200); // Or
        SDL_Rect outerBorder = {boxX - 3, boxY - 3, boxWidth + 6, boxHeight + 6};
        SDL_RenderFillRect(renderer, &outerBorder);

        // Bordure moyenne plus foncée
        SDL_SetRenderDrawColor(renderer, 180, 140, 0, 220);
        SDL_Rect midBorder = {boxX - 2, boxY - 2, boxWidth + 4, boxHeight + 4};
        SDL_RenderFillRect(renderer, &midBorder);

        // Fond de la fenêtre
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 230); // Bleu très foncé
        SDL_Rect box = {boxX, boxY, boxWidth, boxHeight};
        SDL_RenderFillRect(renderer, &box);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        // Texte "SCORE" en petit
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        // Dessiner "SCORE" en pixels (5 lettres x 5 pixels de large + espaces)
        int labelX = boxX + 10;
        int labelY = boxY + 8;

        // S
        for (int py = 0; py < 7; py++) {
            for (int px = 0; px < 5; px++) {
                bool drawPixel = (py == 0 && px > 0) ||
                               (px == 0 && py > 0 && py < 3) ||
                               (py == 3 && px > 0 && px < 4) ||
                               (px == 4 && py > 3 && py < 6) ||
                               (py == 6 && px < 4);
                if (drawPixel) {
                    SDL_RenderDrawPoint(renderer, labelX + px, labelY + py);
                }
            }
        }

        // C
        labelX += 7;
        for (int py = 0; py < 7; py++) {
            for (int px = 0; px < 5; px++) {
                bool drawPixel = ((py == 0 || py == 6) && px > 0 && px < 5) ||
                               (px == 0 && py > 0 && py < 6);
                if (drawPixel) {
                    SDL_RenderDrawPoint(renderer, labelX + px, labelY + py);
                }
            }
        }

        // O
        labelX += 7;
        for (int py = 0; py < 7; py++) {
            for (int px = 0; px < 5; px++) {
                bool drawPixel = ((py == 0 || py == 6) && px > 0 && px < 4) ||
                               ((px == 0 || px == 4) && py > 0 && py < 6);
                if (drawPixel) {
                    SDL_RenderDrawPoint(renderer, labelX + px, labelY + py);
                }
            }
        }

        // R
        labelX += 7;
        for (int py = 0; py < 7; py++) {
            for (int px = 0; px < 5; px++) {
                bool drawPixel = (px == 0) ||
                               (py == 0 && px < 4) ||
                               (py == 3 && px < 4) ||
                               (px == 4 && py > 0 && py < 3) ||
                               (py > 3 && px == (py - 3));
                if (drawPixel) {
                    SDL_RenderDrawPoint(renderer, labelX + px, labelY + py);
                }
            }
        }

        // E
        labelX += 7;
        for (int py = 0; py < 7; py++) {
            for (int px = 0; px < 5; px++) {
                bool drawPixel = (px == 0) ||
                               (py == 0 && px < 5) ||
                               (py == 3 && px < 4) ||
                               (py == 6 && px < 5);
                if (drawPixel) {
                    SDL_RenderDrawPoint(renderer, labelX + px, labelY + py);
                }
            }
        }

        // Afficher le score en grand et en jaune (aligné à gauche dans la boîte)
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        drawNumber(renderer, displayScore, boxX + 10, boxY + 28, 3);
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
        int finalTime = static_cast<int>(totalTime + elapsedTime);
        drawNumber(renderer, finalTime, screenWidth / 2 - 50, centerY, 4);

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        int finalScore = totalScore + getScore();
        drawNumber(renderer, finalScore, screenWidth / 2 - 80, centerY + 80, 4);
    }
}

void Room::drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, int size) {
    // Fonction pour dessiner du texte pixel art
    // Chaque caractère fait 8x8 pixels de base, multiplié par size

    int charWidth = 8 * size;
    int spacing = 2 * size;
    int currentX = x;

    for (char c : text) {
        // Dessiner un rectangle pour chaque caractère
        if (c != ' ') {
            int pixelSize = size;

            // Dessiner une forme de base pour chaque lettre
            for (int py = 0; py < 7; py++) {
                for (int px = 0; px < 5; px++) {
                    bool drawPixel = false;

                    // Patterns pour les lettres
                    switch (c) {
                        case 'A': case 'a':
                            drawPixel = (py == 0 && px > 0 && px < 4) ||
                                       (py > 0 && py < 7 && (px == 0 || px == 4)) ||
                                       (py == 3 && px > 0 && px < 4);
                            break;
                        case 'E': case 'e':
                            drawPixel = (px == 0) ||
                                       (py == 0) ||
                                       (py == 3 && px < 4) ||
                                       (py == 6);
                            break;
                        case 'I': case 'i':
                            drawPixel = (px == 2) ||
                                       (py == 0) ||
                                       (py == 6);
                            break;
                        case 'J': case 'j':
                            drawPixel = (py == 0) ||
                                       (px == 3 && py < 6) ||
                                       (py == 6 && px < 4) ||
                                       (px == 0 && py == 5);
                            break;
                        case 'L': case 'l':
                            drawPixel = (px == 0) ||
                                       (py == 6);
                            break;
                        case 'N': case 'n':
                            drawPixel = (px == 0) ||
                                       (px == 4) ||
                                       (py == px && px < 5);
                            break;
                        case 'O': case 'o':
                            drawPixel = (py == 0 && px > 0 && px < 4) ||
                                       (py == 6 && px > 0 && px < 4) ||
                                       ((px == 0 || px == 4) && py > 0 && py < 6);
                            break;
                        case 'R': case 'r':
                            drawPixel = (px == 0) ||
                                       (py == 0 && px < 4) ||
                                       (py == 3 && px < 4) ||
                                       (px == 4 && py > 0 && py < 3) ||
                                       (py - 3 == px && px > 0);
                            break;
                        case 'S': case 's':
                            drawPixel = (py == 0 && px > 0) ||
                                       (px == 0 && py > 0 && py < 3) ||
                                       (py == 3 && px > 0 && px < 4) ||
                                       (px == 4 && py > 3 && py < 6) ||
                                       (py == 6 && px < 4);
                            break;
                        case 'T': case 't':
                            drawPixel = (py == 0) ||
                                       (px == 2);
                            break;
                        case 'U': case 'u':
                            drawPixel = ((px == 0 || px == 4) && py < 6) ||
                                       (py == 6 && px > 0 && px < 4);
                            break;
                        case 'C': case 'c':
                            drawPixel = (py == 0 && px > 0) ||
                                       (py == 6 && px > 0) ||
                                       (px == 0 && py > 0 && py < 6);
                            break;
                        case 'D': case 'd':
                            drawPixel = (px == 0) ||
                                       (py == 0 && px < 4) ||
                                       (py == 6 && px < 4) ||
                                       (px == 4 && py > 0 && py < 6);
                            break;
                        case '\'':
                            drawPixel = (px == 2 && py < 2);
                            break;
                        default:
                            // Pour les caractères non définis, dessiner un bloc simple
                            drawPixel = (px == 2 && py > 1 && py < 5);
                            break;
                    }

                    if (drawPixel) {
                        for (int sy = 0; sy < pixelSize; sy++) {
                            for (int sx = 0; sx < pixelSize; sx++) {
                                SDL_RenderDrawPoint(renderer,
                                                  currentX + px * pixelSize + sx,
                                                  y + py * pixelSize + sy);
                            }
                        }
                    }
                }
            }
        }

        currentX += charWidth + spacing;
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
