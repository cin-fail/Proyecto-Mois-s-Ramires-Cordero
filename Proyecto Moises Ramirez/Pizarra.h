#pragma once
#include <SFML/Graphics.hpp>

class Board {
public:
    Board();
    void clear();
    void set(int r, int c, int t);
    int get(int r, int c) const;
    static sf::Vector2f cellCenter(int r, int c);

    int grid[8][8];
};

