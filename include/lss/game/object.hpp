#ifndef __OBJECT_H_
#define __OBJECT_H_

#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"

class Object {
public:
    Object();
    virtual ~Object() = default;
    bool passThrough = true;
    bool seeThrough = true;

    std::shared_ptr<Cell> currentCell;
    std::shared_ptr<Location> currentLocation;

    virtual bool interact() = 0;
};


#endif // __OBJECT_H_
