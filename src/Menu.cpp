#include "Menu.hpp"
#include "ScoreManager.hpp"
#include <cmath>
#include <string>

Menu::Menu()
    : state(MenuState::MAIN_MENU),
      selectedOption(0),
      numOptions(3),
      startNewGame(false),
      continueGame(false),
      quit(false),
      difficulty(Difficulty::MEDIUM),
      titlePulse(0.0f),
      titlePulseSpeed(0.05f) {}

Menu::~Menu() {}

void Menu::handleInput(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        if (state == MenuState::MAIN_MENU) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption--;
                    if (selectedOption < 0) {
                        selectedOption = numOptions - 1;
                    }
                    break;

                case SDLK_DOWN:
                    selectedOption++;
                    if (selectedOption >= numOptions) {
                        selectedOption = 0;
                    }
                    break;

                case SDLK_RIGHT:
                    // Aller à l'écran des meilleurs scores
                    state = MenuState::HIGHSCORES_SCREEN;
                    break;

                case SDLK_RETURN:
                case SDLK_SPACE:
                    switch (selectedOption) {
                        case 0: // Jouer - aller à la sélection de difficulté
                            state = MenuState::DIFFICULTY_SELECTION;
                            selectedOption = 1; // Sélectionner MOYEN par défaut
                            numOptions = 3;
                            break;
                        case 1: // Crédits
                            state = MenuState::CREDITS_SCREEN;
                            break;
                        case 2: // Quitter
                            quit = true;
                            break;
                    }
                    break;

                case SDLK_ESCAPE:
                    // Ne rien faire sur le menu principal
                    break;
            }
        } else if (state == MenuState::DIFFICULTY_SELECTION) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption--;
                    if (selectedOption < 0) {
                        selectedOption = numOptions - 1;
                    }
                    break;

                case SDLK_DOWN:
                    selectedOption++;
                    if (selectedOption >= numOptions) {
                        selectedOption = 0;
                    }
                    break;

                case SDLK_RETURN:
                case SDLK_SPACE:
                    // Définir la difficulté selon la sélection
                    switch (selectedOption) {
                        case 0:
                            difficulty = Difficulty::EASY;
                            break;
                        case 1:
                            difficulty = Difficulty::MEDIUM;
                            break;
                        case 2:
                            difficulty = Difficulty::HARD;
                            break;
                    }
                    startNewGame = true;
                    state = MenuState::PLAYING;
                    break;

                case SDLK_ESCAPE:
                    // Retour au menu principal
                    state = MenuState::MAIN_MENU;
                    selectedOption = 0;
                    numOptions = 4;
                    break;
            }
        } else if (state == MenuState::CREDITS_SCREEN) {
            // Retour au menu avec n'importe quelle touche
            if (event.key.keysym.sym == SDLK_ESCAPE ||
                event.key.keysym.sym == SDLK_RETURN ||
                event.key.keysym.sym == SDLK_SPACE) {
                state = MenuState::MAIN_MENU;
            }
        } else if (state == MenuState::HIGHSCORES_SCREEN) {
            // Retour au menu avec n'importe quelle touche
            if (event.key.keysym.sym == SDLK_ESCAPE ||
                event.key.keysym.sym == SDLK_RETURN ||
                event.key.keysym.sym == SDLK_SPACE ||
                event.key.keysym.sym == SDLK_LEFT) {
                state = MenuState::MAIN_MENU;
            }
        }
    }
}

void Menu::update() {
    // Animation du titre
    titlePulse += titlePulseSpeed;
    if (titlePulse > 2 * M_PI) {
        titlePulse -= 2 * M_PI;
    }
}

void Menu::drawFilledRect(SDL_Renderer* renderer, int x, int y, int w, int h) {
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void Menu::drawBorder(SDL_Renderer* renderer, int x, int y, int w, int h) {
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &rect);
}

void Menu::drawText(SDL_Renderer* renderer, const std::string& text, int x, int y,
                    int size, bool selected) {
    // Fonction simple pour dessiner du texte pixel art
    // Chaque caractère fait 8x8 pixels de base, multiplié par size

    int charWidth = 8 * size;
    int charHeight = 8 * size;
    int spacing = 2 * size;

    int currentX = x;
    int textWidth = 0;

    // Calculer la largeur totale du texte
    for (size_t i = 0; i < text.length(); i++) {
        textWidth += charWidth + spacing;
    }

    for (char c : text) {
        // Dessiner un rectangle pour chaque caractère (style rétro simplifié)
        if (c != ' ') {
            // Couleur du texte
            if (selected) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            }

            // Dessiner des rectangles pour former des lettres simplifiées
            // Version très simplifiée - juste des blocs pour chaque lettre
            int pixelSize = size;

            // Dessiner une forme de base pour chaque lettre
            for (int py = 0; py < 7; py++) {
                for (int px = 0; px < 5; px++) {
                    bool drawPixel = false;

                    // Patterns simplifiés pour quelques lettres communes
                    switch (c) {
                        case 'A': case 'a':
                            drawPixel = (py == 0 && px > 0 && px < 4) ||
                                       (py > 0 && py < 7 && (px == 0 || px == 4)) ||
                                       (py == 3 && px > 0 && px < 4);
                            break;
                        case 'B': case 'b':
                            drawPixel = (px == 0) ||
                                       (py == 0 && px < 4) ||
                                       (py == 3 && px < 4) ||
                                       (py == 6 && px < 4) ||
                                       (px == 4 && (py == 1 || py == 2 || py == 4 || py == 5));
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
                        case 'F': case 'f':
                            drawPixel = (px == 0) ||
                                       (py == 0) ||
                                       (py == 3 && px < 3);
                            break;
                        case 'G': case 'g':
                            drawPixel = (py == 0 && px > 0) ||
                                       (py == 6 && px > 0 && px < 4) ||
                                       (px == 0 && py > 0 && py < 6) ||
                                       (py == 3 && px > 2) ||
                                       (px == 4 && py > 3 && py < 6);
                            break;
                        case 'H': case 'h':
                            drawPixel = (px == 0 || px == 4) ||
                                       (py == 3);
                            break;
                        case 'K': case 'k':
                            drawPixel = (px == 0) ||
                                       (py == px && px > 1 && py < 4) ||
                                       (py + px == 6 && px > 1);
                            break;
                        case 'M': case 'm':
                            drawPixel = (px == 0 || px == 4) ||
                                       (py == 1 && (px == 1 || px == 3)) ||
                                       (py == 2 && px == 2);
                            break;
                        case 'P': case 'p':
                            drawPixel = (px == 0) ||
                                       (py == 0 && px < 4) ||
                                       (py == 3 && px < 4) ||
                                       (px == 4 && py > 0 && py < 3);
                            break;
                        case 'Q': case 'q':
                            drawPixel = (py == 0 && px > 0 && px < 4) ||
                                       (py == 6 && px > 0 && px < 4) ||
                                       ((px == 0 || px == 4) && py > 0 && py < 6) ||
                                       (py == 5 && px == 3) ||
                                       (py == 6 && px == 4);
                            break;
                        case 'V': case 'v':
                            drawPixel = (px == 0 && py < 5) ||
                                       (px == 4 && py < 5) ||
                                       (px == 1 && py == 5) ||
                                       (px == 3 && py == 5) ||
                                       (px == 2 && py == 6);
                            break;
                        case 'W': case 'w':
                            drawPixel = (px == 0 || px == 4) ||
                                       (py == 5 && (px == 1 || px == 3)) ||
                                       (py == 6 && px == 2);
                            break;
                        case 'X': case 'x':
                            drawPixel = (py == px) ||
                                       (py + px == 6);
                            break;
                        case 'Y': case 'y':
                            drawPixel = (py == px && py < 3) ||
                                       (py + px == 6 && py < 3) ||
                                       (px == 2 && py > 2);
                            break;
                        case 'Z': case 'z':
                            drawPixel = (py == 0) ||
                                       (py == 6) ||
                                       (py + px == 6);
                            break;
                        case '0':
                            drawPixel = ((py == 0 || py == 6) && px > 0 && px < 4) ||
                                       ((px == 0 || px == 4) && py > 0 && py < 6);
                            break;
                        case '1':
                            drawPixel = (px == 2) ||
                                       (py == 6 && px > 0 && px < 4) ||
                                       (py == 1 && px == 1);
                            break;
                        case '2':
                            drawPixel = (py == 0 && px > 0 && px < 4) ||
                                       (py == 3 && px > 0 && px < 4) ||
                                       (py == 6 && px >= 0 && px < 5) ||
                                       (px == 4 && py > 0 && py < 3) ||
                                       (px == 0 && py > 3 && py < 6);
                            break;
                        case '3':
                            drawPixel = ((py == 0 || py == 3 || py == 6) && px > 0 && px < 4) ||
                                       (px == 4 && py > 0 && py < 6 && py != 3);
                            break;
                        case '4':
                            drawPixel = (px == 0 && py < 4) ||
                                       (py == 3 && px < 5) ||
                                       (px == 4);
                            break;
                        case '5':
                            drawPixel = (py == 0 && px >= 0 && px < 5) ||
                                       (py == 3 && px > 0 && px < 4) ||
                                       (py == 6 && px > 0 && px < 4) ||
                                       (px == 0 && py > 0 && py < 3) ||
                                       (px == 4 && py > 3 && py < 6);
                            break;
                        case '6':
                            drawPixel = ((py == 0 || py == 3 || py == 6) && px > 0 && px < 4) ||
                                       (px == 0 && py > 0 && py < 6) ||
                                       (px == 4 && py > 3 && py < 6);
                            break;
                        case '7':
                            drawPixel = (py == 0) ||
                                       (px == 4 && py > 0 && py < 4) ||
                                       (px == 3 && py > 3);
                            break;
                        case '8':
                            drawPixel = ((py == 0 || py == 3 || py == 6) && px > 0 && px < 4) ||
                                       ((px == 0 || px == 4) && py > 0 && py < 6 && py != 3);
                            break;
                        case '9':
                            drawPixel = ((py == 0 || py == 3 || py == 6) && px > 0 && px < 4) ||
                                       (px == 4 && py > 0 && py < 6) ||
                                       (px == 0 && py > 0 && py < 3);
                            break;
                        default:
                            // Pour les caractères non définis, dessiner un bloc simple
                            drawPixel = (px == 2 && py > 1 && py < 5);
                            break;
                    }

                    if (drawPixel) {
                        drawFilledRect(renderer,
                                     currentX + px * pixelSize,
                                     y + py * pixelSize,
                                     pixelSize, pixelSize);
                    }
                }
            }
        }

        currentX += charWidth + spacing;
    }

    // Dessiner le soulignement blanc si l'option est sélectionnée
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int underlineY = y + charHeight + 4 * size;
        int underlineHeight = 2 * size;
        drawFilledRect(renderer, x, underlineY, textWidth - spacing, underlineHeight);
    }
}

void Menu::drawNumber(SDL_Renderer* renderer, int number, int x, int y, int size) {
    // Convertir le nombre en string
    std::string numStr = std::to_string(number);

    int currentX = x;
    int digitWidth = size * 6;
    int digitSpacing = size * 2;

    for (char digit : numStr) {
        // Vérifier que c'est bien un chiffre
        if (digit < '0' || digit > '9') {
            // Si ce n'est pas un chiffre, sauter mais ne rien dessiner
            currentX += digitWidth + digitSpacing;
            continue;
        }

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

void Menu::renderHighScoresScreen(SDL_Renderer* renderer) {
    // Titre
    // "MEILLEURS SCORES" = 16 chars, size 3 -> largeur = 10*3*16 = 480px -> x_centre = (800-480)/2 = 160
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Couleur dorée
    drawText(renderer, "MEILLEURS SCORES", 160, 50, 3, false);

    int y = 150;
    int spacing = 100;

    // Récupérer les scores
    int scoreEasy = ScoreManager::getInstance().getHighScore(Difficulty::EASY);
    int scoreMedium = ScoreManager::getInstance().getHighScore(Difficulty::MEDIUM);
    int scoreHard = ScoreManager::getInstance().getHighScore(Difficulty::HARD);

    // Afficher FACILE
    // "FACILE" = 6 chars, size 2 -> largeur = 10*2*6 = 120px -> x_centre = (800-120)/2 = 340
    SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255); // Vert clair
    drawText(renderer, "FACILE", 340, y, 2, false);
    y += 40;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
    // Calculer la largeur du nombre pour le centrer
    std::string easyStr = std::to_string(scoreEasy);
    int easyWidth = easyStr.length() * (3 * 6 + 3 * 2); // size=3: digitWidth=18, digitSpacing=6
    int easyX = (800 - easyWidth) / 2;
    drawNumber(renderer, scoreEasy, easyX, y, 3);
    y += spacing;

    // Afficher MOYEN
    // "MOYEN" = 5 chars, size 2 -> largeur = 10*2*5 = 100px -> x_centre = (800-100)/2 = 350
    SDL_SetRenderDrawColor(renderer, 255, 200, 100, 255); // Orange
    drawText(renderer, "MOYEN", 350, y, 2, false);
    y += 40;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
    // Calculer la largeur du nombre pour le centrer
    std::string mediumStr = std::to_string(scoreMedium);
    int mediumWidth = mediumStr.length() * (3 * 6 + 3 * 2);
    int mediumX = (800 - mediumWidth) / 2;
    drawNumber(renderer, scoreMedium, mediumX, y, 3);
    y += spacing;

    // Afficher DIFFICILE
    // "DIFFICILE" = 9 chars, size 2 -> largeur = 10*2*9 = 180px -> x_centre = (800-180)/2 = 310
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255); // Rouge clair
    drawText(renderer, "DIFFICILE", 310, y, 2, false);
    y += 40;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
    // Calculer la largeur du nombre pour le centrer
    std::string hardStr = std::to_string(scoreHard);
    int hardWidth = hardStr.length() * (3 * 6 + 3 * 2);
    int hardX = (800 - hardWidth) / 2;
    drawNumber(renderer, scoreHard, hardX, y, 3);

    // Instructions en bas
    // "ECHAP OU FLECHE GAUCHE POUR RETOUR" = 35 chars, size 1 -> largeur = 10*1*35 = 350px -> x_centre = (800-350)/2 = 225
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    drawText(renderer, "ECHAP OU FLECHE GAUCHE POUR RETOUR", 225, 550, 1, false);
}

void Menu::render(SDL_Renderer* renderer) {
    // Fond noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (state == MenuState::MAIN_MENU) {
        // Titre du jeu avec taille fixe
        int titleSize = 4;

        // Calculer la position centrée du titre
        // "AMAZING BALL" = 12 chars, size 4 -> largeur approximative = 10*4*12 = 480px
        int titleWidth = 10 * titleSize * 12;
        int titleX = (800 - titleWidth) / 2;
        int titleY = 70;

        // Effet de halo coloré arc-en-ciel autour du titre
        int titleHeight = 7 * titleSize;  // Hauteur réelle du texte (7 pixels de hauteur par caractère)
        for (int i = 0; i < 5; i++) {
            float haloPhase = titlePulse * 0.5f + i * 0.3f;
            int r = static_cast<int>(128 + 127 * std::sin(haloPhase));
            int g = static_cast<int>(128 + 127 * std::sin(haloPhase + 2.0f));
            int b = static_cast<int>(128 + 127 * std::sin(haloPhase + 4.0f));
            int alpha = 40 - i * 8;

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
            int offset = (5 - i) * 4;
            SDL_Rect haloRect = {titleX - offset, titleY - offset, titleWidth + offset * 2, titleHeight + offset * 2};
            SDL_RenderFillRect(renderer, &haloRect);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        // Ombre portée du titre (décalée légèrement en bas à droite)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        drawText(renderer, "AMAZING BALL", titleX + 3, titleY + 3, titleSize, false);

        // Titre principal avec dégradé arc-en-ciel animé
        std::string title = "AMAZING BALL";
        for (size_t i = 0; i < title.length(); i++) {
            // Calculer la couleur pour chaque lettre (effet arc-en-ciel qui défile)
            float colorPhase = titlePulse * 2.0f + i * 0.5f;
            int r = static_cast<int>(128 + 127 * std::sin(colorPhase));
            int g = static_cast<int>(128 + 127 * std::sin(colorPhase + 2.0f));
            int b = static_cast<int>(128 + 127 * std::sin(colorPhase + 4.0f));

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);

            // Dessiner chaque lettre individuellement
            std::string letter(1, title[i]);
            int letterX = titleX + i * 10 * titleSize;
            drawText(renderer, letter, letterX, titleY, titleSize, false);
        }

        // Effet de brillance qui traverse le titre périodiquement
        float shinePos = std::fmod(titlePulse * 0.3f, 2.0f);
        if (shinePos < 1.0f) {
            int shineX = static_cast<int>(titleX + shinePos * titleWidth);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
            for (int i = -10; i <= 10; i++) {
                int alpha = 100 - std::abs(i) * 10;
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
                SDL_RenderDrawLine(renderer, shineX + i * 2, titleY - 10, shineX + i * 2, titleY + 100);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        // Options du menu (toutes centrées)
        int menuY = 250;
        int menuSpacing = 70;

        // "JOUER" = 5 chars, size 2 -> largeur = 10*2*5 = 100px -> x_centre = (800-100)/2 = 350
        drawText(renderer, "JOUER", 350, menuY, 2, selectedOption == 0);
        // "CREDITS" = 7 chars, size 2 -> largeur = 10*2*7 = 140px -> x_centre = (800-140)/2 = 330
        drawText(renderer, "CREDITS", 330, menuY + menuSpacing, 2, selectedOption == 1);
        // "QUITTER" = 7 chars, size 2 -> largeur = 10*2*7 = 140px -> x_centre = (800-140)/2 = 330
        drawText(renderer, "QUITTER", 330, menuY + menuSpacing * 2, 2, selectedOption == 2);

        // Instructions en bas
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        // "FLECHES HAUT BAS   ENTREE POUR VALIDER" = 39 chars, size 1 -> largeur = 10*1*39 = 390px -> x_centre = (800-390)/2 = 205
        drawText(renderer, "FLECHES HAUT BAS   ENTREE POUR VALIDER", 205, 530, 1, false);

        // Indication pour accéder aux meilleurs scores
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Couleur dorée
        // "FLECHE DROITE POUR MEILLEURS SCORES" = 36 chars, size 1 -> largeur = 10*1*36 = 360px -> x_centre = (800-360)/2 = 220
        drawText(renderer, "FLECHE DROITE POUR MEILLEURS SCORES", 220, 550, 1, false);

    } else if (state == MenuState::DIFFICULTY_SELECTION) {
        // Écran de sélection de difficulté
        // "CHOISIR DIFFICULTE" = 18 chars, size 3 -> largeur = 10*3*18 = 540px -> x_centre = (800-540)/2 = 130
        drawText(renderer, "CHOISIR DIFFICULTE", 130, 100, 3, false);

        int menuY = 250;
        int menuSpacing = 80;

        // Options de difficulté (toutes centrées)
        // "FACILE" = 6 chars, size 2 -> largeur = 10*2*6 = 120px -> x_centre = (800-120)/2 = 340
        drawText(renderer, "FACILE", 340, menuY, 2, selectedOption == 0);
        // "MOYEN" = 5 chars, size 2 -> largeur = 10*2*5 = 100px -> x_centre = (800-100)/2 = 350
        drawText(renderer, "MOYEN", 350, menuY + menuSpacing, 2, selectedOption == 1);
        // "DIFFICILE" = 9 chars, size 2 -> largeur = 10*2*9 = 180px -> x_centre = (800-180)/2 = 310
        drawText(renderer, "DIFFICILE", 310, menuY + menuSpacing * 2, 2, selectedOption == 2);

        // Instructions en bas
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        // "FLECHES   ENTREE   ECHAP POUR RETOUR" = 37 chars, size 1 -> largeur = 10*1*37 = 370px -> x_centre = (800-370)/2 = 215
        drawText(renderer, "FLECHES   ENTREE   ECHAP POUR RETOUR", 215, 550, 1, false);

    } else if (state == MenuState::CREDITS_SCREEN) {
        // Écran des crédits
        // Calculer les positions centrées (écran 800px de large)
        // Format: (charWidth + spacing) * nb_chars = (8*size + 2*size) * nb_chars = 10*size * nb_chars

        // "CREDITS" = 7 chars, size 3 -> largeur = 10*3*7 = 210px -> x_centre = (800-210)/2 = 295
        drawText(renderer, "CREDITS", 295, 50, 3, false);

        int y = 150;
        int spacing = 50;

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        // "DEVELOPPEMENT" = 13 chars, size 2 -> largeur = 10*2*13 = 260px -> x_centre = (800-260)/2 = 270
        drawText(renderer, "DEVELOPPEMENT", 270, y, 2, false);
        y += spacing;
        // "BRUNO BOIRIE" = 12 chars, size 1 -> largeur = 10*1*12 = 120px -> x_centre = (800-120)/2 = 340
        drawText(renderer, "BRUNO BOIRIE", 340, y, 1, false);
        y += spacing * 2;

        // "GRAPHISMES" = 10 chars, size 2 -> largeur = 10*2*10 = 200px -> x_centre = (800-200)/2 = 300
        drawText(renderer, "GRAPHISMES", 300, y, 2, false);
        y += spacing;
        drawText(renderer, "BRUNO BOIRIE", 340, y, 1, false);
        y += spacing * 2;

        // "MOTEUR" = 6 chars, size 2 -> largeur = 10*2*6 = 120px -> x_centre = (800-120)/2 = 340
        drawText(renderer, "MOTEUR", 340, y, 2, false);
        y += spacing;
        // "SDL2" = 4 chars, size 1 -> largeur = 10*1*4 = 40px -> x_centre = (800-40)/2 = 380
        drawText(renderer, "SDL2", 380, y, 1, false);

        // Instruction de retour
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        // "APPUYER SUR ECHAP POUR REVENIR" = 30 chars, size 1 -> largeur = 10*1*30 = 300px -> x_centre = (800-300)/2 = 250
        drawText(renderer, "APPUYER SUR ECHAP POUR REVENIR", 250, 550, 1, false);
    } else if (state == MenuState::HIGHSCORES_SCREEN) {
        renderHighScoresScreen(renderer);
    }
}
