#include "Game.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "Enemy.hpp"
#include "Menu.hpp"
#include "Room.hpp"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), isRunning(false), lightTexture(nullptr), lightRadius(150), gameStarted(false), inRoom(false), currentLevel(1), windowWidth(800), windowHeight(600), totalScore(0), totalTime(0.0f), playerLives(3), gameOver(false) {}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Erreur SDL_Init: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title,
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width, height,
                               SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Erreur SDL_CreateWindow: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        std::cerr << "Erreur SDL_CreateRenderer: " << SDL_GetError() << std::endl;
        return false;
    }

    isRunning = true;

    windowWidth = width;
    windowHeight = height;

    // Créer le menu
    menu = std::make_unique<Menu>();

    // Initialiser le joueur au centre de l'écran
    player = std::make_unique<Player>(width / 2, height / 2);

    // Créer une carte simple
    map = std::make_unique<Map>();

    // Carte de test 20x15 (pour une fenêtre 800x600 avec tiles 40x40)
    int level1[20 * 15] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1,
        1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
        1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
        1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
        1,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1,
        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    };

    map->loadMap(level1, 20, 15);

    // Créer quelques ennemis
    enemies.push_back(std::make_unique<Enemy>(200, 150));
    enemies.push_back(std::make_unique<Enemy>(600, 200));
    enemies.push_back(std::make_unique<Enemy>(400, 400));
    enemies.push_back(std::make_unique<Enemy>(150, 450));

    // Créer la texture de lumière
    createLightTexture();

    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        if (!gameStarted) {
            // Gérer les entrées du menu
            menu->handleInput(event);

            // Vérifier si le jeu doit démarrer
            if (menu->shouldStartNewGame()) {
                gameStarted = true;
                inRoom = true;
                currentLevel = 1;
                totalScore = 0; // Réinitialiser le score total
                totalTime = 0.0f; // Réinitialiser le temps total
                playerLives = 3; // Réinitialiser les vies
                gameOver = false; // Réinitialiser le game over
                menu->resetFlags();

                // Réinitialiser le jeu pour une nouvelle partie
                // Placer le joueur dans la zone de départ (à gauche)
                player = std::make_unique<Player>(80, windowHeight / 2);

                // Créer la première salle avec la difficulté choisie
                currentRoom = std::make_unique<Room>(windowWidth, windowHeight, currentLevel, menu->getDifficulty());

                // Pas d'ennemis dans le mode salle
                enemies.clear();
            } else if (menu->shouldContinueGame()) {
                gameStarted = true;
                menu->resetFlags();
                // Continuer avec l'état actuel du jeu
            } else if (menu->shouldQuit()) {
                isRunning = false;
            }
        } else {
            // Gérer les entrées du jeu
            player->handleEvent(event);

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    // Retour au menu
                    gameStarted = false;
                    menu->setState(MenuState::MAIN_MENU);
                }
            }
        }
    }
}

void Game::update() {
    if (!gameStarted) {
        // Mettre à jour le menu
        menu->update();
        return;
    }

    player->handleInput();
    player->update();

    Vector2D playerPos = player->getPosition();

    if (inRoom && currentRoom) {
        // Mettre à jour la salle (timer, particules, animations)
        currentRoom->update(1.0f / 60.0f); // deltaTime approximatif

        // Démarrer le timer si le joueur quitte la zone de départ
        static bool hasStarted = false;
        if (!hasStarted && playerPos.x > 100) {
            currentRoom->startTimer();
            hasStarted = true;
        }

        // Vérifier si le joueur est tombé dans un trou
        if (currentRoom->isPlayerInHole(playerPos, player->getRadius()) && !gameOver) {
            // Perdre une vie
            playerLives--;
            if (playerLives <= 0) {
                gameOver = true;
                currentRoom->stopTimer();
            }
            // Réinitialiser le joueur à la position de départ
            player = std::make_unique<Player>(80, windowHeight / 2);
            // Ne PAS réinitialiser hasStarted - le timer continue
        }

        // Vérifier si le joueur a atteint la fin de la salle
        if (currentRoom->hasReachedEnd(playerPos) && !currentRoom->isCelebrating() && !gameOver) {
            // Arrêter le timer et créer l'explosion de fête
            currentRoom->stopTimer();
            currentRoom->createCelebrationParticles(playerPos);
        }

        // Si la célébration est terminée, passer au niveau suivant
        if (currentRoom->isCelebrating() && currentRoom->getScore() >= 0 && !gameOver) {
            static float celebrationTimer = 0.0f;
            celebrationTimer += 1.0f / 60.0f;

            if (celebrationTimer > 2.0f) { // Attendre 2 secondes
                celebrationTimer = 0.0f;
                hasStarted = false;

                // Ajouter le score et le temps du niveau actuel au total
                totalScore += currentRoom->getScore();
                totalTime += currentRoom->getElapsedTime();

                // Passer au niveau suivant
                currentLevel++;

                // Réinitialiser le joueur à la position de départ
                player = std::make_unique<Player>(80, windowHeight / 2);

                // Créer la salle suivante
                currentRoom = std::make_unique<Room>(windowWidth, windowHeight, currentLevel, menu->getDifficulty());
            }
        }
    } else {
        // Mode exploration avec ennemis (ancien mode)
        // Mettre à jour les ennemis
        for (auto& enemy : enemies) {
            enemy->update(playerPos);
        }

        // Détection de collision entre l'attaque du joueur et les ennemis
        if (player->isAttacking()) {
            Direction playerDir = player->getDirection();
            int attackRange = player->getAttackRange();

            for (auto& enemy : enemies) {
                Vector2D enemyPos = enemy->getPosition();
                float dx = enemyPos.x - playerPos.x;
                float dy = enemyPos.y - playerPos.y;

                // Vérifier si l'ennemi est dans la direction de l'attaque
                bool inAttackZone = false;

                switch (playerDir) {
                    case Direction::UP:
                        inAttackZone = (dy < 0 && std::abs(dy) <= attackRange && std::abs(dx) <= 15);
                        break;
                    case Direction::DOWN:
                        inAttackZone = (dy > 0 && std::abs(dy) <= attackRange && std::abs(dx) <= 15);
                        break;
                    case Direction::LEFT:
                        inAttackZone = (dx < 0 && std::abs(dx) <= attackRange && std::abs(dy) <= 15);
                        break;
                    case Direction::RIGHT:
                        inAttackZone = (dx > 0 && std::abs(dx) <= attackRange && std::abs(dy) <= 15);
                        break;
                }

                if (inAttackZone) {
                    enemy->takeDamage(1);
                }
            }
        }

        // Supprimer les ennemis morts
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                [](const std::unique_ptr<Enemy>& e) { return e->isDead(); }),
            enemies.end()
        );
    }
}

void Game::createLightTexture() {
    // Créer une texture de la taille de l'écran pour le masque de lumière
    lightTexture = SDL_CreateTexture(renderer,
                                      SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_TARGET,
                                      800, 600);

    SDL_SetTextureBlendMode(lightTexture, SDL_BLENDMODE_MOD);
}

void Game::drawPlayerLight(int playerX, int playerY) {
    // Dessiner sur la texture (une seule fois par frame)
    SDL_SetRenderTarget(renderer, lightTexture);

    // Remplir avec du noir (tout cacher)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Optimisation: dessiner le gradient avec des cercles au lieu de pixels
    int numCircles = 50;  // Nombre de cercles concentriques

    for (int i = numCircles; i >= 0; i--) {
        float t = static_cast<float>(i) / numCircles;
        int currentRadius = static_cast<int>(lightRadius * t);

        // Calculer la luminosité pour ce cercle
        int brightness = static_cast<int>(255.0f * (1.0f - t * t * t));

        SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255);

        // Dessiner un cercle rempli optimisé
        int y1 = playerY - currentRadius;
        int y2 = playerY + currentRadius;

        for (int y = y1; y <= y2; y++) {
            int dy = y - playerY;
            int halfWidth = static_cast<int>(std::sqrt(currentRadius * currentRadius - dy * dy));

            // Dessiner une ligne horizontale au lieu de points individuels
            SDL_RenderDrawLine(renderer,
                             playerX - halfWidth, y,
                             playerX + halfWidth, y);
        }
    }

    // Revenir au rendu normal
    SDL_SetRenderTarget(renderer, nullptr);

    // Appliquer la texture sur tout l'écran
    SDL_RenderCopy(renderer, lightTexture, nullptr, nullptr);
}

void Game::render() {
    // Fond noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (!gameStarted) {
        // Afficher le menu
        menu->render(renderer);
    } else if (inRoom && currentRoom) {
        // Mode salle : afficher la salle
        currentRoom->render(renderer);

        // Appliquer le masque de lumière avec SDL_BLENDMODE_MOD
        // Cela multiplie les couleurs : noir (0,0,0) cache tout, blanc (255,255,255) révèle
        Vector2D playerPos = player->getPosition();
        drawPlayerLight(static_cast<int>(playerPos.x), static_cast<int>(playerPos.y));

        // Rendre le joueur APRÈS pour qu'il soit visible
        player->render(renderer);

        // Afficher le HUD (score et temps) en premier plan, après l'effet de lumière
        currentRoom->renderHUD(renderer, totalScore, totalTime, playerLives, gameOver);
    } else {
        // Mode exploration (ancien mode)
        // Rendre la carte
        map->render(renderer);

        // Rendre les ennemis
        for (auto& enemy : enemies) {
            enemy->render(renderer);
        }

        // Appliquer le masque de lumière avec SDL_BLENDMODE_MOD
        // Cela multiplie les couleurs : noir (0,0,0) cache tout, blanc (255,255,255) révèle
        Vector2D playerPos = player->getPosition();
        drawPlayerLight(static_cast<int>(playerPos.x), static_cast<int>(playerPos.y));

        // Rendre le joueur APRÈS pour qu'il soit visible
        player->render(renderer);
    }

    SDL_RenderPresent(renderer);

    // Contrôle du framerate
    SDL_Delay(frameDelay);
}

void Game::clean() {
    if (lightTexture) {
        SDL_DestroyTexture(lightTexture);
        lightTexture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}
