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
  std::string name;

  bool interact(std::shared_ptr<Object>);
  std::string getTitle();
  std::string getFullTitle();
};

class TorchStand : public Object {
public:
  TorchStand() : Object(), type(ItemType::TORCH_STAND) {}
  ItemSpec type;
};

typedef std::vector<std::shared_ptr<Item>> Items;


namespace Prototype {
  const std::shared_ptr<Item> TORCH = std::make_shared<Item>(ItemType::TORCH, Effects{std::make_shared<VisibilityModifier>(2.5f)} );
  const std::shared_ptr<Item> PLATE = std::make_shared<Item>(ItemType::PLATE, Effects{std::make_shared<ArmorValue>(5)} );
}

#endif // __ITEM_H_
