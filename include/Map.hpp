#ifndef MAP_HPP
#define MAP_HPP

#include <SDL2/SDL.h>
#include <vector>

class Map {
public:
    Map();
    ~Map();

    void loadMap(int arr[], int sizeX, int sizeY);
    void render(SDL_Renderer* renderer);

    bool isWall(int x, int y) const;

private:
    std::vector<std::vector<int>> mapData;
    int tileSize;
    int mapWidth;
    int mapHeight;
};

#endif
