#ifndef __EQUIPMENT_H_
#define __EQUIPMENT_H_
#include "lss/game/slot.hpp"
 // = {
 //            std::make_shared<Slot>({WEAPON}),
 //            std::make_shared<Slot>({WEAPON}),
 //            std::make_shared<Slot>({WEAPON_TWOHANDED}),
 //        };


typedef std::vector<std::shared_ptr<Slot>> Slots;
class Equipment {
  Equipment();
  Slots slots;
  bool equip(std::shared_ptr<Item>);
  bool unequip(std::shared_ptr<Item>);
};


#endif // __EQUIPMENT_H_
