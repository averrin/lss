#ifndef __ITEM_H_
#define __ITEM_H_
#include "lss/game/effect.hpp"
#include "lss/game/itemSpec.hpp"
#include "lss/game/object.hpp"
#include "lss/game/randomTools.hpp"
#include <variant>

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
    item->effects.clear();
    for (auto e : effects) {
      auto ne = e->clone();
      ne->roll();
      item->effects.push_back(ne);
    }
    return item;
  }

  std::shared_ptr<Item> setCount(int count) {
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
    ItemType::PLATE, Effects{std::make_shared<ArmorValue>(R::F(3, 5))});
const auto LEATHER_ARMOR = std::make_shared<Item>(
    ItemType::LEATHER_ARMOR, Effects{std::make_shared<ArmorValue>(R::F(1, 3))});
const auto HELMET = std::make_shared<Item>(
    ItemType::HELMET, Effects{std::make_shared<ArmorValue>(R::F(1, 3))});
const auto SHIELD = std::make_shared<Item>(
    ItemType::SHIELD, Effects{std::make_shared<ArmorValue>(R::F(2, 3))});
const auto GREAVES = std::make_shared<Item>(
    ItemType::GREAVES, Effects{std::make_shared<ArmorValue>(R::F(1, 3))});
const auto BOOTS = std::make_shared<Item>(
    ItemType::BOOTS, Effects{std::make_shared<ArmorValue>(R::F(1, 3))});

const auto SPEED_RING = std::make_shared<Item>(
    ItemType::GOLD_RING,
    Effects{std::make_shared<SpecialPostfix>("of lightning"),
            std::make_shared<SpeedModifier>(0.3)});

const auto SWORD = std::make_shared<Item>(
    ItemType::SWORD, Effects{std::make_shared<MeleeDamage>(
                         R::F(-2, 2), R::I(2, 4), R::I(2, 4))});
const auto GREAT_AXE = std::make_shared<Item>(
    ItemType::GREAT_AXE, Effects{std::make_shared<MeleeDamage>(-1, 6, 7),
                                 std::make_shared<SpeedModifier>(-0.3f)});
const auto DAGGER = std::make_shared<Item>(
    ItemType::DAGGER,
    Effects{std::make_shared<MeleeDamage>(R::F(-2, 2), 1, R::I(1, 3))});
const auto GOLD = std::make_shared<Item>(ItemType::GOLD_COINS, 1);
} // namespace Prototype

#endif // __ITEM_H_
