#include "Room.hpp"
#include "Player.hpp"
#include <cstdlib>
#include <ctime>
#include <cmath>

Room::Room(int screenWidth, int screenHeight, int level, Difficulty difficulty)
    : screenWidth(screenWidth),
      screenHeight(screenHeight),
      level(level),
      difficulty(difficulty),
      endZoneX(screenWidth - 100) {

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

void Room::render(SDL_Renderer* renderer) {
    // Fond de la salle (couleur légèrement différente)
    SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderClear(renderer);

    // Dessiner la zone de départ (gauche) en vert
    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
    SDL_Rect startZone = {0, 0, 100, screenHeight};
    SDL_RenderFillRect(renderer, &startZone);

    // Dessiner la zone d'arrivée (droite) en bleu
    SDL_SetRenderDrawColor(renderer, 0, 0, 150, 255);
    SDL_Rect endZone = {screenWidth - 100, 0, 100, screenHeight};
    SDL_RenderFillRect(renderer, &endZone);

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

    // Afficher le niveau en haut
    // (On utilisera du texte simple pour l'instant)
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
