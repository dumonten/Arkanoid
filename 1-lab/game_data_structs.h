#ifndef GAME_DATA_STRUCTS_H
#define GAME_DATA_STRUCTS_H

#include <iostream>
#include <vector>

namespace sf{
    typedef int Vector2f;
    typedef int Vector2i;
    typedef int Color;
    class Drawable{};
    class RenderStates{};
    class RenderTarget{};
    class Text{};
    class Font{};
    class Uint32{};
    class Event{};
    class RenderWindow{};
    class Sprite{};
};

namespace gui
{
     enum state
        {
            clicked,
            hovered,
            normal
        };
}

enum gameState
        {
    on,
    off,
    wait
        };

struct statistics
{
    uint16_t numOfLives;
    uint32_t score, numOfBonuses, numOfCrashedBlocks, deadTime;
    std::string userName;
};

enum difficulty
{
    HARD,
    MEDIUM,
    LOW
};

typedef sf::Vector2i resolution;
const std::vector<std::pair<int, int>> mResolution
{
        {1920, 1080}
};


#endif //GAME_DATA_STRUCTS_H
