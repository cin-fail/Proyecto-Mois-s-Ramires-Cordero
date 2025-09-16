#include "Pizarra.h"

static const float BOARD_OFFSET_X_LOCAL = 40.f;
static const float BOARD_OFFSET_Y_LOCAL = 40.f;
static const float CELL_SIZE_LOCAL2 = 56.f;

Board::Board() { 
    clear(); 
}

void Board::clear() {
    for (int r = 0; r < 8; ++r) for (int c = 0; c < 8; ++c) grid[r][c] = -1;
}

void Board::set(int r, int c, int t) {
    if (r >= 0 && r < 8 && c >= 0 && c < 8) grid[r][c] = t;
}

int Board::get(int r, int c) const {
    if (r < 0 || r >= 8 || c < 0 || c >= 8) return -1;
    return grid[r][c];
}

sf::Vector2f Board::cellCenter(int r, int c) {
    return sf::Vector2f(BOARD_OFFSET_X_LOCAL + c * CELL_SIZE_LOCAL2 + CELL_SIZE_LOCAL2 / 2.f,
        BOARD_OFFSET_Y_LOCAL + r * CELL_SIZE_LOCAL2 + CELL_SIZE_LOCAL2 / 2.f);
}

//Probandi shit