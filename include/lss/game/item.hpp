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
  std::shared_ptr<Item> roll() {
    auto item = std::make_shared<Item>(*this);
    item->count = count <= 1 ? count : rand() % count + 1;
    return item;
  }
  std::shared_ptr<Item> clone(int count) {
    auto item = std::make_shared<Item>(*this);
    item->count = count <= 1 ? count : rand() % count + 1;
    return item;
  }
};

class TorchStand : public Object {
public:
  TorchStand() : Object(), type(ItemType::TORCH_STAND) { seeThrough = true; }
  ItemSpec type;
};

class Grass : public Object {
public:
  Grass() : Object(), type(ItemType::GRASS) { seeThrough = true; }
  ItemSpec type;
};

typedef std::vector<std::shared_ptr<Item>> Items;

// TODO: consumables
// TODO: randomized items

namespace Prototype {
const auto TORCH = std::make_shared<Item>(
    ItemType::TORCH, Effects{std::make_shared<VisibilityModifier>(2.5f)});
const auto PLATE = std::make_shared<Item>(
    ItemType::PLATE, Effects{std::make_shared<ArmorValue>(3)});
const auto LEATHER_ARMOR = std::make_shared<Item>(
    ItemType::LEATHER_ARMOR, Effects{std::make_shared<ArmorValue>(1)});
const auto HELMET = std::make_shared<Item>(
    ItemType::HELMET, Effects{std::make_shared<ArmorValue>(1)});
const auto SHIELD = std::make_shared<Item>(
    ItemType::SHIELD, Effects{std::make_shared<ArmorValue>(2)});
const auto GREAVES = std::make_shared<Item>(
    ItemType::GREAVES, Effects{std::make_shared<ArmorValue>(1)});
const auto BOOTS = std::make_shared<Item>(
    ItemType::BOOTS, Effects{std::make_shared<ArmorValue>(1)});

const auto SPEED_RING = std::make_shared<Item>(
        ItemType::GOLD_RING,
        Effects{std::make_shared<SpecialPostfix>("of lightning"),
                std::make_shared<SpeedModifier>(0.3)});


const auto GREAT_AXE = std::make_shared<Item>(
    ItemType::GREAT_AXE, Effects{std::make_shared<MeleeDamage>(-1, 6, 7),
                                 std::make_shared<SpeedModifier>(-0.3f)});
const auto DAGGER = std::make_shared<Item>(
    ItemType::DAGGER, Effects{std::make_shared<MeleeDamage>(1, 2, 2)});
  const auto GOLD = std::make_shared<Item>(ItemType::GOLD_COINS, 1);
} // namespace Prototype

#endif // __ITEM_H_
