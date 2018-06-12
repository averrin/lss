#ifndef __CREATURE_H_
#define __CREATURE_H_
#include <algorithm>

#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"

enum Direction {
    N, E, S, W
};

class Creature {
public:
    Creature();

    std::shared_ptr<Cell> currentCell;
    float visibilityDistance = 10.5f;
    std::vector<std::shared_ptr<Cell>> viewField;
    std::shared_ptr<Location> currentLocation;

    bool move(Direction);
    bool canSee(std::shared_ptr<Cell> c) {
        return std::find(viewField.begin(), viewField.end(), c) != viewField.end();
    };
    void calcViewField();
};

#endif // __CREATURE_H_
