#ifndef __ITEM_H_
#define __ITEM_H_
#include "lss/game/object.hpp"
#include "lss/game/effect.hpp"

#include "fmt/ostream.h"

struct ItemSpec {
public:
  std::string name;
  bool equipable = false;

  friend bool operator<(const ItemSpec &lhs, const ItemSpec &rhs) {
    return lhs.name < rhs.name;
  }
  friend bool operator==(ItemSpec &lhs, const ItemSpec &rhs) {
    return lhs.name == rhs.name;
  }
};

namespace ItemType {
ItemSpec const CORPSE = {"corpse", false};
ItemSpec const ROCK = {"rock", false};
ItemSpec const PICK_AXE = {"pick axe", true};
ItemSpec const SWORD = {"rusty sword", true};
};

class Item : public Object {
public:
  Item(ItemSpec);
  ItemSpec type;
        
  Effects effects;

  bool interact(std::shared_ptr<Object>);

  friend std::ostream &operator<<(std::ostream &os, const Item &item) {
    os << item.type.name;
    return os;
  }
};

#endif // __ITEM_H_
