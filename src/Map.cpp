#include "Map.hpp"

Map::Map() : tileSize(40), mapWidth(0), mapHeight(0) {}

Map::~Map() {}

void Map::loadMap(int arr[], int sizeX, int sizeY) {
    mapWidth = sizeX;
    mapHeight = sizeY;

    mapData.resize(mapHeight);
    for (int i = 0; i < mapHeight; i++) {
        mapData[i].resize(mapWidth);
        for (int j = 0; j < mapWidth; j++) {
            mapData[i][j] = arr[i * mapWidth + j];
        }
    }
}

void Map::render(SDL_Renderer* renderer) {
    for (int i = 0; i < mapHeight; i++) {
        for (int j = 0; j < mapWidth; j++) {
            SDL_Rect tileRect = {j * tileSize, i * tileSize, tileSize, tileSize};

            if (mapData[i][j] == 1) {
                // Mur - gris
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderFillRect(renderer, &tileRect);
            } else {
                // Sol - vert foncé
                SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
                SDL_RenderFillRect(renderer, &tileRect);
            }

            // Bordure des tiles
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderDrawRect(renderer, &tileRect);
        }
    }
}

bool Map::isWall(int x, int y) const {
    int tileX = x / tileSize;
    int tileY = y / tileSize;

    if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapHeight) {
        return true;
    }

    return mapData[tileY][tileX] == 1;
}
