#ifndef __ITEM_H_
#define __ITEM_H_
#include "lss/game/object.hpp"
#include "lss/game/effect.hpp"
#include "lss/game/itemSpec.hpp"

#include "fmt/ostream.h"

class Item : public Object {
public:
  Item(ItemSpec);
  Item(ItemSpec, Effects);
  ItemSpec type;
  Effects effects;

  bool equipped = false;

  bool interact(std::shared_ptr<Object>);

  friend std::ostream &operator<<(std::ostream &os, const Item &item) {
    os << item.type.name;
    return os;
  }
};

typedef std::vector<std::shared_ptr<Item>> Items;

#endif // __ITEM_H_
