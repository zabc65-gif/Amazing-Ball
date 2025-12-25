#include "Game.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), isRunning(false), lightTexture(nullptr), lightRadius(150) {}

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
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            }
        }
    }
}

void Game::update() {
    player->handleInput();
    player->update();
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

    // Rendre la carte
    map->render(renderer);

    // Appliquer le masque de lumière avec SDL_BLENDMODE_MOD
    // Cela multiplie les couleurs : noir (0,0,0) cache tout, blanc (255,255,255) révèle
    Vector2D playerPos = player->getPosition();
    drawPlayerLight(static_cast<int>(playerPos.x), static_cast<int>(playerPos.y));

    // Rendre le joueur APRÈS pour qu'il soit visible
    player->render(renderer);

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
