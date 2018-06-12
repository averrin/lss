#include "lss/game/player.hpp"

Player::Player() {}

bool Player::move(Direction d) {
    auto cells = currentLocation->cells;
    auto cc = currentCell;
    switch (d) {
        case N:
            currentCell = cells[cc->y-1][cc->x];
            break;
        case E:
            currentCell = cells[cc->y][cc->x+1];
            break;
        case S:
            currentCell = cells[cc->y+1][cc->x];
            break;
        case W:
            currentCell = cells[cc->y][cc->x-1];
            break;
    }
    return true;
}
