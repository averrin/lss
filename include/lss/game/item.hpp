#ifndef __ITEM_H_
#define __ITEM_H_
#include "lss/game/effect.hpp"
#include "lss/game/itemSpec.hpp"
#include "lss/game/object.hpp"

class Item : public Object {
public:
  Item(ItemSpec);
  Item(ItemSpec, int);
  Item(ItemSpec, Effects);
  ItemSpec type;
  Effects effects;
  int count = 0;

  bool equipped = false;

  bool interact(std::shared_ptr<Object>);
  std::string getTitle();
  std::string getFullTitle();
};

typedef std::vector<std::shared_ptr<Item>> Items;

#endif // __ITEM_H_
