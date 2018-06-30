#ifndef __CREATURE_H_
#define __CREATURE_H_
#include <algorithm>

#include "lss/game/equipment.hpp"
#include "lss/game/item.hpp"
#include "lss/game/object.hpp"

class Creature : public Object {
public:
  Creature();
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
  std::string name = "Unnamed";

  Items inventory;
  std::shared_ptr<Equipment> equipment;
  bool pick(std::shared_ptr<Item>);
  bool drop(std::shared_ptr<Item>);

  int hp;
  int hp_max;
  int damage_dices;
  int damage_edges;
  int damage_modifier;
  float speed = 1.f;
  float visibilityDistance = 5.5f;

  std::shared_ptr<Attribute> HP =
      std::make_shared<Attribute>(AttributeType::HP);
  std::shared_ptr<Attribute> HP_MAX =
      std::make_shared<Attribute>(AttributeType::HP_MAX);
  std::shared_ptr<Attribute> SPEED =
      std::make_shared<Attribute>(AttributeType::SPEED);
  std::shared_ptr<Attribute> VISIBILITY_DISTANCE =
      std::make_shared<Attribute>(AttributeType::VISIBILITY_DISTANCE);

  std::shared_ptr<Cell> getCell(Direction);
};

#endif // __CREATURE_H_
