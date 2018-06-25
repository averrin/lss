#ifndef __CREATURE_H_
#define __CREATURE_H_
#include <algorithm>

#include "lss/game/object.hpp"

class Creature : public Object {
public:
  Creature();
  float visibilityDistance = 5.5f;
  std::vector<std::shared_ptr<Cell>> viewField;
  std::shared_ptr<Location> currentLocation;

  bool move(Direction, bool autoAction = false);
  bool attack(Direction);
  bool canSee(std::shared_ptr<Cell> c) {
    return std::find(viewField.begin(), viewField.end(), c) != viewField.end();
  };
  void calcViewField();
  bool interact(std::shared_ptr<Object>);
  int getDamage(std::shared_ptr<Object>);

  // TODO: add setters-getters for attributes. Keep effects in mind
  // TODO: move equip-inventory logic here. get loot from inventory and apply
  // effects from items
  int hp;
  int hp_max;
  int damage_dices;
  int damage_edges;
  int damage_modifier;
  float speed = 1.f;

  std::shared_ptr<Cell> getCell(Direction);
};

#endif // __CREATURE_H_
