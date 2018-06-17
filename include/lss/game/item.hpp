#ifndef __ITEM_H_
#define __ITEM_H_
#include "lss/game/object.hpp"

#include "fmt/ostream.h"

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

	friend std::ostream& operator<<(std::ostream& os, const Item& item)
	{
		os << item.name;
		return os;
	}
};

#endif // __ITEM_H_
