#ifndef __ITEM_H_
#define __ITEM_H_
#include "lss/game/effect.hpp"
#include "lss/game/itemSpec.hpp"
#include "lss/game/object.hpp"
#include "lss/game/randomTools.hpp"
#include "lss/game/spell.hpp"
#include "lss/utils.hpp"
#include <fmt/format.h>
#include <variant>

class Item : public Object {
public:
  Item(std::string n, ItemSpec t)
      : Object(), type(t), unidName(type.name), name(n),
        durability(type.durability) {
    zIndex = 1;
  }
  Item(std::string un, std::string n, ItemSpec t)
      : Object(), type(t), unidName(un), name(n), durability(type.durability) {
    zIndex = 1;
  }
  Item(ItemSpec t)
      : Object(), type(t), unidName(type.name), name(t.name),
        durability(type.durability) {
    zIndex = 1;
  }
  Item(ItemSpec t, int c)
      : Object(), type(t), count(c), unidName(t.name), name(t.name),
        durability(type.durability) {
    zIndex = 1;
  }
  Item(std::string n, ItemSpec t, int c)
      : Object(), type(t), count(c), unidName(t.name), name(n),
        durability(type.durability) {
    zIndex = 1;
  }
  Item(ItemSpec t, Effects e)
      : Object(), type(t), effects(e), unidName(t.name), name(t.name),
        durability(type.durability) {
    zIndex = 1;
  }
  Item(std::string n, ItemSpec t, Effects e)
      : Object(), type(t), effects(e), unidName(t.name), name(n),
        durability(type.durability) {
    zIndex = 1;
  }
  ItemSpec type;
  Effects effects;
  int count = 0;
  int durability;

  bool equipped = false;
  bool identified = false;
  std::string unidName;
  std::string name;

  bool interact(std::shared_ptr<Object>);
  std::string getTitle(bool force = false);
  std::string getFullTitle();

  virtual std::shared_ptr<Item> clone() {
    return std::make_shared<Item>(*this);
  }
  virtual std::shared_ptr<Item> roll() {
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

class Consumable : public Item {
public:
  Consumable(std::string un, std::string n, ItemSpec t,
             std::shared_ptr<Spell> s)
      : Item(un, n, t), spell(s) {}
  std::shared_ptr<Spell> spell;
  std::shared_ptr<Item> clone() { return std::make_shared<Consumable>(*this); }
  std::shared_ptr<Item> roll() { return std::make_shared<Consumable>(*this); }
};

typedef std::vector<std::shared_ptr<Item>> Items;

namespace Prototype {
const auto TORCH = std::make_shared<Item>(
    ItemType::TORCH, Effects{std::make_shared<VisibilityModifier>(2.5f)});
const auto PLATE = std::make_shared<Item>(
    ItemType::PLATE, Effects{std::make_shared<ArmorValue>(R::I(3, 5))});
const auto GOD_PLATE = std::make_shared<Item>(
    "GoD pLaTe", ItemType::PLATE,
    Effects{std::make_shared<TraitEffect>(Traits::INVULNERABLE)});
const auto BASIC_LEATHER_ARMOR = std::make_shared<Item>(
    ItemType::LEATHER_ARMOR, Effects{std::make_shared<ArmorValue>(1)});
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

const auto BOOTS_OF_FLY =
    std::make_shared<Item>("flying boots", ItemType::BOOTS,
                           Effects{std::make_shared<TraitEffect>(Traits::FLY)});

const auto GRASS = std::make_shared<Item>("just herb", ItemType::GRASS, 1);

const auto SPEED_RING =
    std::make_shared<Item>("ring of lightning", ItemType::GOLD_RING,
                           Effects{std::make_shared<SpeedModifier>(0.3)});

const auto BASIC_SWORD = std::make_shared<Item>(
    ItemType::SWORD, Effects{std::make_shared<MeleeDamage>(0, 3, 3)});
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
const auto POISON_DAGGER = std::make_shared<Item>(
    ItemType::DAGGER,
    Effects{std::make_shared<MeleeDamage>(2, 3, 3),
            std::make_shared<CritModifier>(0.2),
            std::make_shared<OnHitEffect>(
                std::make_shared<LastingEffect>(OverTimeEffects::POISON, 5000),
                0.3)});

const auto POISON_FANG = std::make_shared<Item>(
    ItemType::ENEMY,
    Effects{std::make_shared<MeleeDamage>(2, 3, 3),
            std::make_shared<CritModifier>(0.2),
            std::make_shared<OnHitEffect>(
                std::make_shared<LastingEffect>(OverTimeEffects::POISON, 5000),
                0.3)});

const auto SWORD_OF_DUALWIELD = std::make_shared<Item>(
    "sword of dualwield", ItemType::SWORD,
    Effects{std::make_shared<MeleeDamage>(R::I(-2, 2), R::I(2, 4), R::I(2, 4)),
            std::make_shared<TraitEffect>(Traits::DUAL_WIELD)});

// TODO: make colors unique
const auto POTION_HEAL_LESSER = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "lesser heal potion",
    ItemType::POTION, Spells::HEAL_LESSER);
const auto POTION_HEAL = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "heal potion",
    ItemType::POTION, Spells::HEAL);

const auto POTION_MANA = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "restore mana potion",
    ItemType::POTION, Spells::RESTORE_MANA);

const auto POTION_GOD_SPEED = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "god speed potion",
    ItemType::POTION, Spells::GOD_SPEED);

const auto POTION_HP_BOOST = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "health boost potion",
    ItemType::POTION, Spells::HP_BOOST);

const auto POTION_VISIBILITY_BOOST = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()),
    "visibility boost potion", ItemType::POTION, Spells::VISIBILITY_BOOST);

const auto POTION_CRIT_BOOST = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()),
    "crit chance boost potion", ItemType::POTION, Spells::CRIT_BOOST);

const auto POTION_LEVITATION = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "levitation potion",
    ItemType::POTION, Spells::LEVITATION);

const auto POTION_REGENERATION = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "regeneration potion",
    ItemType::POTION, Spells::EOT_HEAL);

const auto POTION_POISON = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "poison",
    ItemType::POTION, Spells::EOT_POISON);

const auto POTION_CONFUSION = std::make_shared<Consumable>(
    fmt::format("{} potion", utils::getRandomColor()), "potion of confusion",
    ItemType::POTION, Spells::CONFUSION);

const auto SCROLL_IDENTIFICATION = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of identification", ItemType::SCROLL, Spells::IDENTIFY);
const auto SCROLL_REVEAL = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of reveal", ItemType::SCROLL, Spells::REVEAL);
const auto SCROLL_TELEPORT = std::make_shared<Consumable>(
    fmt::format("scroll labled '{}'", utils::getScrollName()),
    "scroll of teleport", ItemType::SCROLL, Spells::TELEPORT_RANDOM);

const auto GOLD = std::make_shared<Item>(ItemType::GOLD_COINS, 1);
} // namespace Prototype

#endif // __ITEM_H_
