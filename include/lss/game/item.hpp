#ifndef __ITEM_H_
#define __ITEM_H_
#include "lss/game/object.hpp"

#include "fmt/ostream.h"

struct ItemSpec {
public:
    std::string name;

	friend bool operator<(const ItemSpec& lhs, const ItemSpec& rhs)
	{
		return lhs.name < rhs.name;
	}
	friend bool operator==(ItemSpec& lhs, const ItemSpec& rhs)
	{
		return lhs.name == rhs.name;
	}
};

namespace ItemType {
  ItemSpec const CORPSE = {"corpse"};
  ItemSpec const ROCK = {"rock"};
  ItemSpec const PICK_AXE = {"pick axe"};
};

class Item : public Object {
public:
  Item(ItemSpec);
  ItemSpec type;

  bool interact();

	friend std::ostream& operator<<(std::ostream& os, const Item& item)
	{
		os << item.type.name;
		return os;
	}
};

#endif // __ITEM_H_
