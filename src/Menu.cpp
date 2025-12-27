#include "Menu.hpp"
#include "ScoreManager.hpp"
#include <cmath>
#include <string>

Menu::Menu()
    : state(MenuState::MAIN_MENU),
      selectedOption(0),
      numOptions(4),
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

                case SDLK_RETURN:
                case SDLK_SPACE:
                    switch (selectedOption) {
                        case 0: // Jouer - aller à la sélection de difficulté
                            state = MenuState::DIFFICULTY_SELECTION;
                            selectedOption = 1; // Sélectionner MOYEN par défaut
                            numOptions = 3;
                            break;
                        case 1: // Continuer
                            continueGame = true;
                            state = MenuState::PLAYING;
                            break;
                        case 2: // Crédits
                            state = MenuState::CREDITS_SCREEN;
                            break;
                        case 3: // Quitter
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

void Menu::render(SDL_Renderer* renderer) {
    // Fond noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (state == MenuState::MAIN_MENU) {
        // Titre du jeu avec effet de pulsation
        float pulse = 1.0f + 0.1f * std::sin(titlePulse);
        int titleSize = static_cast<int>(4 * pulse);

        // Calculer la position centrée du titre
        // "AMAZING BALL" = 12 chars, size ~4 -> largeur approximative = 10*4*12 = 480px
        int titleWidth = 10 * titleSize * 12;
        int titleX = (800 - titleWidth) / 2;

        // Effet de halo autour du titre (centré aussi)
        for (int i = 0; i < 3; i++) {
            int alpha = 50 - i * 15;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
            int offset = (3 - i) * 3;
            // Simuler un halo avec des rectangles
            SDL_Rect haloRect = {titleX - offset, 80 - offset, titleWidth + offset * 2, 80 + offset * 2};
            SDL_RenderDrawRect(renderer, &haloRect);
        }

        drawText(renderer, "AMAZING BALL", titleX, 80, titleSize, false);

        // Options du menu (toutes centrées)
        int menuY = 250;
        int menuSpacing = 70;

        // "JOUER" = 5 chars, size 2 -> largeur = 10*2*5 = 100px -> x_centre = (800-100)/2 = 350
        drawText(renderer, "JOUER", 350, menuY, 2, selectedOption == 0);
        // "CONTINUER" = 9 chars, size 2 -> largeur = 10*2*9 = 180px -> x_centre = (800-180)/2 = 310
        drawText(renderer, "CONTINUER", 310, menuY + menuSpacing, 2, selectedOption == 1);
        // "CREDITS" = 7 chars, size 2 -> largeur = 10*2*7 = 140px -> x_centre = (800-140)/2 = 330
        drawText(renderer, "CREDITS", 330, menuY + menuSpacing * 2, 2, selectedOption == 2);
        // "QUITTER" = 7 chars, size 2 -> largeur = 10*2*7 = 140px -> x_centre = (800-140)/2 = 330
        drawText(renderer, "QUITTER", 330, menuY + menuSpacing * 3, 2, selectedOption == 3);

        // Afficher le meilleur score
        int highScore = ScoreManager::getInstance().getHighScore();
        if (highScore > 0) {
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Couleur dorée
            std::string scoreText = "MEILLEUR SCORE   " + std::to_string(highScore);
            // Calculer la largeur approximative du texte pour le centrer
            int scoreWidth = 10 * 2 * scoreText.length();
            int scoreX = (800 - scoreWidth) / 2;
            drawText(renderer, scoreText, scoreX, 190, 2, false);
        }

        // Instructions en bas
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        // "FLECHES HAUT BAS   ENTREE POUR VALIDER" = 39 chars, size 1 -> largeur = 10*1*39 = 390px -> x_centre = (800-390)/2 = 205
        drawText(renderer, "FLECHES HAUT BAS   ENTREE POUR VALIDER", 205, 550, 1, false);

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
    }
}
