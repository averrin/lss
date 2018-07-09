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
  int durability;

  bool equipped = false;
  std::string name;

  bool interact(std::shared_ptr<Object>);
  std::string getTitle();
  std::string getFullTitle();

  std::shared_ptr<Item> clone() { return std::make_shared<Item>(*this); }
};

class TorchStand : public Object {
public:
  TorchStand() : Object(), type(ItemType::TORCH_STAND) { seeThrough = true; }
  ItemSpec type;
};

typedef std::vector<std::shared_ptr<Item>> Items;

// TODO: consumables
// TODO: randomized items

namespace Prototype {
const auto TORCH = std::make_shared<Item>(
    ItemType::TORCH, Effects{std::make_shared<VisibilityModifier>(2.5f)});
const auto PLATE = std::make_shared<Item>(
    ItemType::PLATE, Effects{std::make_shared<ArmorValue>(1)});
const auto GREAT_AXE = std::make_shared<Item>(
    ItemType::GREAT_AXE, Effects{std::make_shared<MeleeDamage>(-1, 6, 7),
                                 std::make_shared<SpeedModifier>(-0.3f)});
const auto DAGGER = std::make_shared<Item>(
    ItemType::DAGGER, Effects{std::make_shared<MeleeDamage>(1, 2, 2)});
} // namespace Prototype

#endif // __ITEM_H_
