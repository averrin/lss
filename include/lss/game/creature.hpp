#ifndef __CREATURE_H_
#define __CREATURE_H_
#include <algorithm>

#include "lss/game/object.hpp"

enum Direction {
    N, E, S, W,
    NW, NE, SW, SE
};

class Creature : public Object {
public:
    Creature();
    float visibilityDistance = 10.5f;
    std::vector<std::shared_ptr<Cell>> viewField;

    bool move(Direction);
    bool canSee(std::shared_ptr<Cell> c) {
        return std::find(viewField.begin(), viewField.end(), c) != viewField.end();
    };
    void calcViewField();
    bool interact();

    int hp;
};

#endif // __CREATURE_H_
