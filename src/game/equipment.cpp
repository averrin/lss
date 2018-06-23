#include "lss/game/equipment.hpp"
#include <iostream>

Equipment::Equipment() {}

bool Equipment::equip(std::shared_ptr<Slot> slot, std::shared_ptr<Item> item) {
    fmt::print("Equipment equip: {} --> {}\n", item->type.name, slot->name);
    slot->equip(item);
    return true;
}
bool Equipment::unequip(std::shared_ptr<Slot> slot) {
    slot->unequip();
    return true;
}


