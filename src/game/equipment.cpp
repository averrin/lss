#include "lss/game/equipment.hpp"
#include <iostream>

Equipment::Equipment() {}

bool Equipment::equip(std::shared_ptr<Slot> slot, std::shared_ptr<Item> item) {
  fmt::print("Equipment equip: {} --> {}\n", item->type.name, slot->name);
  slot->equip(item);
  if (item->type.wearableType == WEAPON_TWOHANDED) {
    auto secondary = std::find_if(
        slots.begin(), slots.end(), [slot](std::shared_ptr<Slot> s) {
          return s != slot &&
                 std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                           WEAPON) != s->acceptTypes.end();
        });
    if (secondary != slots.end()) {
      (*secondary)->equip(item);
    }
  }
  return true;
}
bool Equipment::unequip(std::shared_ptr<Slot> slot) {
  auto item = slot->item;
  slot->unequip();
  if (item->type.wearableType == WEAPON_TWOHANDED) {
    auto secondary = std::find_if(
        slots.begin(), slots.end(), [slot](std::shared_ptr<Slot> s) {
          return s != slot &&
                 std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                           WEAPON) != s->acceptTypes.end();
        });
    if (secondary != slots.end()) {
      (*secondary)->unequip();
    }
  }
  return true;
}
