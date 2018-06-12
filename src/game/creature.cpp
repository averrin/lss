#include <cmath>

#include "lss/game/creature.hpp"

Creature::Creature(){}

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
    }
    auto hasCreature = std::find_if(currentLocation->creatures.begin(),
                                    currentLocation->creatures.end(),
                 [&](std::shared_ptr<Creature> creature){
                     return creature->currentCell == currentCell;
                 }) != currentLocation->creatures.end();
    if (!currentCell->passThrough || hasCreature) {
        currentCell = cc;
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
