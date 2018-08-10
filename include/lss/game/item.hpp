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

// TODO: refactor constructors
class Item : public Object {
public:
  Item(std::string un, std::string n, ItemSpec t, Effects e = {})
      : Object(), type(t), unidName(un), name(n), durability(type.durability),
        effects(e) {
    zIndex = 1;
  }
  Item(ItemSpec t, int c = -1)
      : Object(), type(t), count(c), unidName(t.name), name(t.name),
        durability(type.durability) {
    zIndex = 1;
  }
  Item(std::string n, ItemSpec t, int c = -1)
      : Object(), type(t), count(c), unidName(t.name), name(n),
        durability(type.durability) {
    zIndex = 1;
  }
  Item(ItemSpec t, Effects e)
      : Object(), type(t), effects(e), unidName(t.name), name(t.name),
        durability(type.durability) {
    zIndex = 1;
  }
  Item(ItemSpec t, int c, Effects e)
      : Object(), type(t), effects(e), unidName(t.name), name(t.name),
        durability(type.durability), count(c) {
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

#endif // __ITEM_H_
