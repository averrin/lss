#include <cmath>
#include <algorithm>

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
    if (!currentCell->passThrough) {
        currentCell = cc;
    } else {
        calcViewField();
    }
    return true;
}

void Player::calcViewField() {
    viewField.clear();
    auto cc = currentCell;
    for (auto r : currentLocation->cells) {
        for (auto c : r) {
            auto d = sqrt(pow(cc->x - c->x, 2) + pow(cc->y - c->y, 2));
            if (d >= visibilityDistance) {
                continue;
            }
            viewField.push_back(c);
        }
    }
}

bool Player::canSee(std::shared_ptr<Cell> c) {
    return std::find(viewField.begin(), viewField.end(), c) != viewField.end();
}
