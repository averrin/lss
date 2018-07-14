#ifndef __ITEM_H_
#define __ITEM_H_
#include "lss/game/effect.hpp"
#include "lss/game/itemSpec.hpp"
#include "lss/game/object.hpp"
#include "lss/game/randomTools.hpp"
#include <variant>

class Item : public Object {
public:
  Item(std::string n, ItemSpec t)
      : Object(), type(t), name(n), durability(type.durability) {}
  Item(ItemSpec t)
      : Object(), type(t), name(t.name), durability(type.durability) {}
  Item(ItemSpec t, int c)
      : Object(), type(t), count(c), name(t.name), durability(type.durability) {
  }
  Item(std::string n, ItemSpec t, int c)
      : Object(), type(t), count(c), name(n), durability(type.durability) {}
  Item(ItemSpec t, Effects e)
      : Object(), type(t), effects(e), name(t.name),
        durability(type.durability) {}
  Item(std::string n, ItemSpec t, Effects e)
      : Object(), type(t), effects(e), name(n), durability(type.durability) {}
  ItemSpec type;
  Effects effects;
  int count = 0;
  int durability;

  bool equipped = false;
  bool identified = false;
  std::string name;

  bool interact(std::shared_ptr<Object>);
  std::string getTitle(bool force = false);
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

typedef std::vector<std::shared_ptr<Item>> Items;

// TODO: consumables
// TODO: randomized items

namespace Prototype {
const auto TORCH = std::make_shared<Item>(
    ItemType::TORCH, Effects{std::make_shared<VisibilityModifier>(2.5f)});
const auto PLATE = std::make_shared<Item>(
    ItemType::PLATE, Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto GOD_PLATE =
    std::make_shared<Item>("GoD pLaTe", ItemType::PLATE,
                           Effects{std::make_shared<ArmorValue>(100),
                                   std::make_shared<HPModifier>(1000)});
const auto LEATHER_ARMOR = std::make_shared<Item>(
    ItemType::LEATHER_ARMOR, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto HELMET = std::make_shared<Item>(
    ItemType::HELMET, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto SHIELD = std::make_shared<Item>(
    ItemType::SHIELD, Effects{std::make_shared<ArmorValue>(R::I(2, 3))});
const auto GREAVES = std::make_shared<Item>(
    ItemType::GREAVES, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});
const auto BOOTS = std::make_shared<Item>(
    ItemType::BOOTS, Effects{std::make_shared<ArmorValue>(R::I(1, 3))});

const auto GRASS = std::make_shared<Item>("healing herb", ItemType::GRASS, 1);

const auto SPEED_RING =
    std::make_shared<Item>("ring of lightning", ItemType::GOLD_RING,
                           Effects{std::make_shared<SpeedModifier>(0.3)});

const auto SWORD = std::make_shared<Item>(
    ItemType::SWORD, Effects{std::make_shared<MeleeDamage>(
                         R::I(-2, 2), R::I(2, 4), R::I(2, 4))});
const auto ORK_SWORD = std::make_shared<Item>(
    ItemType::SWORD, Effects{std::make_shared<MeleeDamage>(5, 4, 6),
                             std::make_shared<CritModifier>(0.4)});
const auto GREAT_AXE = std::make_shared<Item>(
    ItemType::GREAT_AXE, Effects{std::make_shared<MeleeDamage>(-1, 6, 7),
                                 std::make_shared<SpeedModifier>(-0.3f)});
const auto DAGGER = std::make_shared<Item>(
    ItemType::DAGGER,
    Effects{std::make_shared<MeleeDamage>(R::I(-2, 2), 1, R::I(1, 3))});
const auto GOBLIN_DAGGER = std::make_shared<Item>(
    ItemType::DAGGER, Effects{std::make_shared<MeleeDamage>(2, 3, 3),
                              std::make_shared<CritModifier>(0.2)});
const auto GOLD = std::make_shared<Item>(ItemType::GOLD_COINS, 1);
} // namespace Prototype

#endif // __ITEM_H_
