#include <cmath>

#include "lss/game/creature.hpp"

Creature::Creature(){
    passThrough = false;
}

bool Creature::interact() {
    return false;
}

bool Creature::move(Direction d) {
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
        case NW:
            currentCell = cells[cc->y-1][cc->x-1];
            break;
        case NE:
            currentCell = cells[cc->y+1][cc->x+1];
            break;
        case SW:
            currentCell = cells[cc->y+1][cc->x-1];
            break;
        case SE:
            currentCell = cells[cc->y+1][cc->x+1];
            break;
    }
    auto obstacle = std::find_if(currentLocation->objects.begin(),
                                    currentLocation->objects.end(),
                 [&](std::shared_ptr<Object> o){
                     return o->currentCell == currentCell && !o->passThrough;
                 });
    auto hasObstacles = obstacle != currentLocation->objects.end();
    if (!currentCell->passThrough || hasObstacles) {
        currentCell = cc;
        if (hasObstacles) {
            return !(*obstacle)->interact();
        }
        return false;
    } else {
        calcViewField();
    }
    return true;
}

void Creature::calcViewField() {
    viewField.clear();
    auto cc = currentCell;
    for (auto r : currentLocation->cells) {
        for (auto c : r) {
            auto d = sqrt(pow(cc->x - c->x, 2) + pow(cc->y - c->y, 2));
            if (d >= visibilityDistance) {
                continue;
            }
            //TODO: implement correct fov computation
            viewField.push_back(c);
        }
    }
}
