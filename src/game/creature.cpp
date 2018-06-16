#include <cmath>

#include "lss/game/creature.hpp"
#include "lss/game/events.hpp"

#include "EventBus.hpp"

Creature::Creature(){
    passThrough = false;
}

bool Creature::interact() {
    return false;
}

LeaveCellEvent::LeaveCellEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c): eb::Event(s), cell(c) {}
EnterCellEvent::EnterCellEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c): eb::Event(s), cell(c) {}

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
    if (!currentCell->passThrough || hasObstacles ) {
        currentCell = cc;
        if (hasObstacles && !(*obstacle)->passThrough) {
            return !(*obstacle)->interact();
        }
        return false;
    } else {
        calcViewField();
    }
    
    LeaveCellEvent le(shared_from_this(), cc);
    eb::EventBus::FireEvent(le);
    EnterCellEvent ee(shared_from_this(), currentCell);
    eb::EventBus::FireEvent(ee);
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
