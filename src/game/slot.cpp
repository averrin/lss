#include "lss/game/slot.hpp"
#include "fmt/format.h"

Slot::Slot(std::string n, std::vector<WearableType> at)
    : Object(), acceptTypes(at), name(n) {}
Slot::Slot(std::string n, std::vector<WearableType> at, bool v)
    : Object(), acceptTypes(at), name(n), visible(v) {}

void Slot::equip(std::shared_ptr<Item> i) {
  // fmt::print("Equip {} in slot {}\n", i->type.name, name);
  item = i;
  item->equipped = true;
}

void Slot::unequip() {
  if (item == nullptr)
    return;
  item->equipped = false;
  item = nullptr;
}
