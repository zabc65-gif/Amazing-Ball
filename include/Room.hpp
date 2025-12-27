#ifndef ROOM_HPP
#define ROOM_HPP

#include <SDL2/SDL.h>
#include <vector>
#include "Vector2D.hpp"
#include "Menu.hpp"

struct Hole {
    Vector2D position;
    int radius;
};

class Room {
public:
    Room(int screenWidth, int screenHeight, int level, Difficulty difficulty);
    ~Room();

    void render(SDL_Renderer* renderer);
    bool isPlayerInHole(const Vector2D& playerPos, int playerRadius) const;
    bool hasReachedEnd(const Vector2D& playerPos) const;

private:
    int screenWidth;
    int screenHeight;
    int level;
    Difficulty difficulty;

    std::vector<Hole> holes;
    int endZoneX;  // Zone à atteindre à droite
    int holeRadius;

    void generateHoles();
};

#endif
