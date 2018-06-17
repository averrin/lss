#ifndef __ITEM_H_
#define __ITEM_H_
#include "lss/game/object.hpp"

enum ItemType {
  CORPSE,
  ROCK,
  PICK_AXE,
};

class Item : public Object {
public:
  Item(ItemType, std::string);
  ItemType type;
  std::string name;

  bool interact();
};

#endif // __ITEM_H_
