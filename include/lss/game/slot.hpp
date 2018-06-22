#ifndef __SLOT_H_
#define __SLOT_H_
#include "lss/game/item.hpp"

enum WearableType {
    INVALID,
    HEAD, NECK,
    LEFT_PAULDRON, RIGHT_PAULDRON,
    WEAPON, WEAPON_TWOHANDED,
    // LEFT_HAND, RIGHT_HAND,
    LEFT_GAUNTLET, RIGHT_GAUNTLET,
    BODY, GREAVES, BOOTS,
    RING, CLOAK,
    TWO_HANDED, LIGHT
};

class Slot {
    Slot(std::vector<WearableType> at): acceptTypes(at) {}
    std::vector<WearableType> acceptTypes;
    std::shared_ptr<Item> item;
    void eqiup(std::shared_ptr<Item> item);
    void uneqiup(std::shared_ptr<Item> item);
};



#endif // __SLOT_H_
