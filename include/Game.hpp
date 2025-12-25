#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <memory>

class Player;
class Map;

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void handleEvents();
    void update();
    void render();
    void clean();

    bool running() const { return isRunning; }

    SDL_Renderer* getRenderer() const { return renderer; }

    void drawPlayerLight(int playerX, int playerY);
    void createLightTexture();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    std::unique_ptr<Player> player;
    std::unique_ptr<Map> map;

    SDL_Texture* lightTexture;
    int lightRadius;

    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
};

#endif
