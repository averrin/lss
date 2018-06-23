#ifndef __EQUIPMENT_H_
#define __EQUIPMENT_H_
#include "lss/game/slot.hpp"

typedef std::vector<std::shared_ptr<Slot>> Slots;
class Equipment {
public:
  Equipment();
  Slots slots;
  bool equip(std::shared_ptr<Slot>, std::shared_ptr<Item>);
  bool unequip(std::shared_ptr<Slot>);
};


#endif // __EQUIPMENT_H_
