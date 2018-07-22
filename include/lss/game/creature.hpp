#ifndef __CREATURE_H_
#define __CREATURE_H_
#include <algorithm>

#include "lss/game/damage.hpp"
#include "lss/game/effect.hpp"
#include "lss/game/equipment.hpp"
#include "lss/game/item.hpp"
#include "lss/game/object.hpp"
#include "lss/game/trait.hpp"
#include <lss/game/location.hpp>

class Creature : public Object {
public:
  Creature();
  std::vector<std::shared_ptr<Cell>> viewField;
  std::shared_ptr<Location> currentLocation;
  std::vector<Trait> traits;
  Effects activeEffects;

  bool move(Direction, bool autoAction = false);
  bool attack(Direction);
  bool canSee(std::shared_ptr<Cell> c) {
    return std::find(viewField.begin(), viewField.end(), c) !=
               viewField.end() ||
           hasTrait(Traits::MIND_SIGHT);
  };
  void calcViewField(bool force = false);
  bool interact(std::shared_ptr<Object>);
  std::shared_ptr<Damage> getDamage(std::shared_ptr<Object>);

  std::string name = "Unnamed";

  std::vector<std::string> knownItems;
  void identify(std::shared_ptr<Item> item) {
    item->identified = true;
    knownItems.push_back(item->getTitle());
    std::for_each(inventory.begin(), inventory.end(), [&](auto i) {
      if (!i->identified && i->getTitle(true) == item->getTitle(true)) {
        identify(i);
      }
    });
  }
  bool isIdentified(std::shared_ptr<Item> item) {
    return item->identified ||
           std::find(knownItems.begin(), knownItems.end(),
                     item->getTitle(true)) != knownItems.end();
  }

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
  float visibility_distance = 5.5f;
  float crit_chance = 0.1;
  int defense = 0;
  int level = 0;

  int mp = 0;
  int mp_max = 0;

  std::optional<std::tuple<std::shared_ptr<Slot>, int, int, int>>
  getPrimaryDmg();
  std::optional<std::tuple<std::shared_ptr<Slot>, int, int, int>>
      getSecondaryDmg(std::shared_ptr<Slot>);
  bool hasTrait(Trait t) {
    auto allTraits = traits;

    for (auto s : equipment->slots) {
      if (s->item == nullptr ||
          std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                    s->item->type.wearableType) == s->acceptTypes.end()) {
        continue;
      }
      for (auto e : s->item->effects) {
        if (auto et = std::dynamic_pointer_cast<TraitEffect>(e)) {
          allTraits.push_back(et->trait);
        }
      }
    }
    for (auto e : activeEffects) {
      if (auto le = std::dynamic_pointer_cast<LastingEffect>(e)) {
        if (auto et = std::dynamic_pointer_cast<TraitEffect>(le->effect)) {
          allTraits.push_back(et->trait);
        }
      }
    }
    return std::find(allTraits.begin(), allTraits.end(), t) != allTraits.end();
  }
  bool hasLight();
  std::shared_ptr<Damage> updateDamage(std::shared_ptr<Damage>, int m, int d,
                                       int e);
  int hitRoll(int m, int d, int e);
  std::shared_ptr<Slot> getSlot(WearableType type);
  std::optional<std::shared_ptr<Slot>> getSlot(WearableType type, bool);

  Attribute HP = Attribute(AttributeType::HP);
  Attribute HP_MAX = Attribute(AttributeType::HP_MAX);
  Attribute MP = Attribute(AttributeType::MP);
  Attribute MP_MAX = Attribute(AttributeType::MP_MAX);

  Attribute SPEED = Attribute(AttributeType::SPEED);
  Attribute VISIBILITY_DISTANCE = Attribute(AttributeType::VISIBILITY_DISTANCE);
  Attribute DMG = Attribute(AttributeType::DAMAGE);
  Attribute DEF = Attribute(AttributeType::DEFENSE);
  Attribute CRIT = Attribute(AttributeType::CRIT_CHANCE);

  std::shared_ptr<Cell> getCell(std::shared_ptr<Cell>, Direction);
  void applyEoT(EoT, int);

private:
  std::shared_ptr<Cell> cachedCell;
};

// namespace EoT {
//   const auto HEAL = std::make_shared<OverTimeEffect>(500,
//     [](std::shared_ptr<Creature> c){
//             c->hp += 5;
//   });
// }

#endif // __CREATURE_H_
