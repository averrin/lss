#ifndef __CREATURE_H_
#define __CREATURE_H_
#include <algorithm>

#include "lss/game/equipment.hpp"
#include "lss/game/item.hpp"
#include "lss/game/object.hpp"
#include "lss/game/trait.hpp"

class Creature : public Object {
public:
  Creature();
  std::vector<std::shared_ptr<Cell>> viewField;
  std::shared_ptr<Location> currentLocation;
  std::vector<Trait> traits;

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
  int defense = 0;

  std::optional<std::tuple<std::shared_ptr<Slot>, int, int, int>> getPrimaryDmg();
  std::optional<std::tuple<std::shared_ptr<Slot>, int, int, int>> getSecondaryDmg(std::shared_ptr<Slot>);
  bool hasTrait(Trait t) {
    return std::find(traits.begin(), traits.end(), t) != traits.end();
  }
  bool hasLight();

  Attribute HP =
      Attribute(AttributeType::HP);
  Attribute HP_MAX =
      Attribute(AttributeType::HP_MAX);
  Attribute SPEED =
      Attribute(AttributeType::SPEED);
  Attribute VISIBILITY_DISTANCE =
      Attribute(AttributeType::VISIBILITY_DISTANCE);
  Attribute DMG =
      Attribute(AttributeType::DAMAGE);
  Attribute DEF =
      Attribute(AttributeType::DEFENSE);

  std::shared_ptr<Cell> getCell(Direction);
};

#endif // __CREATURE_H_
