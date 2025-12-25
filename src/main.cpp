#include "Game.hpp"
#include <iostream>

int main(int, char**) {
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    Game game;

    if (!game.init("Amazing Ball", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        std::cerr << "Échec de l'initialisation du jeu!" << std::endl;
        return -1;
    }

    std::cout << "=== Amazing Ball ===" << std::endl;
    std::cout << "Contrôles:" << std::endl;
    std::cout << "  Flèches directionnelles - Déplacement" << std::endl;
    std::cout << "  Espace - Attaquer" << std::endl;
    std::cout << "  ESC - Quitter" << std::endl;
    std::cout << "======================" << std::endl;

    while (game.running()) {
        game.handleEvents();
        game.update();
        game.render();
    }

    game.clean();
    return 0;
}
