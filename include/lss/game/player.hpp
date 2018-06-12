#ifndef __PLAYER_H_
#define __PLAYER_H_
#include <memory>

#include "lss/game/location.hpp"
#include "lss/game/cell.hpp"

enum Direction {
    N, E, S, W
};

class Player {
public:
    Player();
    std::shared_ptr<Location> currentLocation;
    std::shared_ptr<Cell> currentCell;

    bool move(Direction);
    bool canSee(std::shared_ptr<Cell>);
    void calcViewField();

    float visibilityDistance = 10.5f;
    std::vector<std::shared_ptr<Cell>> viewField;
};


#endif // __PLAYER_H_
