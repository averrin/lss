#ifndef __GENERATOR_H_
#define __GENERATOR_H_
#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"

class Generator {
public:
    Generator();
    std::shared_ptr<Room> getRoom();
    std::shared_ptr<Room> makePassage(std::shared_ptr<Cell>, Direction, int);
    Cells getFloor();
    std::shared_ptr<Location> getLocation();
    void placeRoom();
};


#endif // __GENERATOR_H_
