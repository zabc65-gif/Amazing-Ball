#ifndef MENU_HPP
#define MENU_HPP

#include <SDL2/SDL.h>
#include <string>
#include <vector>

enum class MenuOption {
    PLAY,
    CONTINUE,
    CREDITS,
    QUIT,
    BACK
};

enum class MenuState {
    MAIN_MENU,
    CREDITS_SCREEN,
    PLAYING
};

class Menu {
public:
    Menu();
    ~Menu();

    void handleInput(SDL_Event& event);
    void update();
    void render(SDL_Renderer* renderer);

    MenuState getState() const { return state; }
    void setState(MenuState newState) { state = newState; }

    bool shouldStartNewGame() const { return startNewGame; }
    bool shouldContinueGame() const { return continueGame; }
    bool shouldQuit() const { return quit; }
    void resetFlags() { startNewGame = false; continueGame = false; quit = false; }

private:
    MenuState state;
    int selectedOption;
    int numOptions;

    bool startNewGame;
    bool continueGame;
    bool quit;

    // Animation
    float titlePulse;
    float titlePulseSpeed;

    void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y,
                  int size, bool selected = false);
    void drawFilledRect(SDL_Renderer* renderer, int x, int y, int w, int h);
    void drawBorder(SDL_Renderer* renderer, int x, int y, int w, int h);
};

#endif
