#ifndef __SLOT_H_
#define __SLOT_H_
#include "lss/game/item.hpp"
#include "lss/game/wearableType.hpp"

class Slot: public Object {
public:
    Slot(std::string n, std::vector<WearableType> at);
    Slot(std::string n, std::vector<WearableType> at, bool v);
    std::string name;
    bool visible = true;
    std::vector<WearableType> acceptTypes;
    std::shared_ptr<Item> item;
    void equip(std::shared_ptr<Item> item);
    void unequip();
};



#endif // __SLOT_H_
