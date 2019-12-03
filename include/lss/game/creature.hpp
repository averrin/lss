#ifndef __CREATURE_H_
#define __CREATURE_H_
#include <algorithm>
#include <thread>
#include <liblog/liblog.hpp>
#include "EventBus.hpp"

#include "lss/game/content/traits.hpp"
#include "lss/game/damage.hpp"
#include "lss/game/effect.hpp"
#include "lss/game/equipment.hpp"
#include "lss/game/item.hpp"
#include "lss/game/object.hpp"
#include "lss/game/trait.hpp"
#include <lss/game/ai.hpp>
#include <lss/game/damageSpec.hpp>
#include <lss/game/location.hpp>

class Creature : public Object {
  LibLog::Logger &log = LibLog::Logger::getInstance();
public:
  Creature();
  std::string getId() {return name;}
  std::vector<std::shared_ptr<Cell>> viewField;
  std::shared_ptr<Location> currentLocation;
  std::vector<Trait> traits;
  Effects activeEffects;

  std::thread bgThread;
  bool move(Direction, bool autoAction = false);
  bool attack(Direction);
  bool canSee(std::shared_ptr<Cell> c) {
    return canSee(c, viewField);
  };
  bool canSee(std::shared_ptr<Cell> c, std::vector<std::shared_ptr<Cell>> vf) {
    for (auto t : utils::castObjects<Terrain>(
             currentLocation->getObjects(currentCell))) {
      if (t->type == TerrainType::DARKNESS) {
        return false;
      }
    }

    return std::find(vf.begin(), vf.end(), c) !=
               vf.end() ||
           hasTrait(Traits::MIND_SIGHT);
  };
  std::vector<std::shared_ptr<Cell>> calcViewField(bool force = false);
  bool interact(std::shared_ptr<Object>);
  std::shared_ptr<Damage> getDamage(std::shared_ptr<Object>);
  std::string getDmgDesc();

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
  bool throwItem(std::shared_ptr<Item>, std::shared_ptr<Cell>);

  int hp;
  int hp_max;
  DamageSpec dmgSpec = DamageSpec(0, 0, 0, DamageType::BASIC);
  float speed = 1.f;
  float visibility_distance = 0;
  float default_visibility = 3.2f;
  float crit_chance = 0.1;
  int defense = 0;
  int level = 0;
  int throw_distance = 7;

  int mp = 0;
  int mp_max = 0;

  int actionPoints = 0;

  std::optional<AiAction> execAction() {
    log.warn("call execAction on creature");
    log.info(fmt::format("creature [{}@{}.{}]", lu::magenta(name),
                                  currentCell->x, currentCell->y));
    return std::nullopt;
  }

  std::optional<std::tuple<std::shared_ptr<Slot>, DamageSpec>> getPrimaryDmg();
  std::optional<std::tuple<std::shared_ptr<Slot>, DamageSpec>>
      getSecondaryDmg(std::shared_ptr<Slot>);

  std::vector<Trait> getTraits() {
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
    return allTraits;
  }
  bool hasTrait(Trait t) {
    auto allTraits = getTraits();

    return std::find(allTraits.begin(), allTraits.end(), t) != allTraits.end();
  }
  bool hasLight();
  std::shared_ptr<Damage> updateDamage(std::shared_ptr<Damage>, DamageSpec);
  std::vector<std::shared_ptr<Cell>> getInRadius(float distance);

  void applyDamage(std::shared_ptr<Object> attacker, std::shared_ptr<Damage>);

  virtual void onDamage(std::shared_ptr<Creature>, std::shared_ptr<Damage>) = 0;
  virtual void onDie() = 0;

  std::shared_ptr<Slot> getSlot(WearableType type);
  std::optional<std::shared_ptr<Slot>> getSlot(WearableType type, bool);
  bool hasResist(DamageType dt) {
    auto trait = RESISTS.at(dt);
    return hasTrait(trait);
  }

  bool hasImmunity(DamageType dt) {
    auto trait = IMMUNITIES.at(dt);
    return hasTrait(trait);
  }

  bool hasVulnerable(DamageType dt) {
    auto trait = VULNERABLES.at(dt);
    return hasTrait(trait);
  }

  std::optional<std::shared_ptr<Creature>> getNearestTarget(float distance) {
    std::optional<std::shared_ptr<Creature>> target = std::nullopt;
    auto cells = getInRadius(distance);
    std::vector<std::shared_ptr<Creature>> targets;
    for (auto c : cells) {
      auto objects = currentLocation->getObjects(c);
      auto enemies = utils::castObjects<Creature>(objects);
      if (enemies.size() != 0) {
        targets.push_back(enemies.front());
      }
    }
    auto d = distance;
    auto cc = currentCell;
    for (auto e : targets) {
      if (e->currentCell == nullptr) {
        throw std::runtime_error(
            fmt::format("{} -> nullptr current cell again", e->name));
      }
      auto td = sqrt(pow(cc->x - e->currentCell->x, 2) +
                     pow(cc->y - e->currentCell->y, 2));
      if (td <= d) {
        target = std::make_optional<std::shared_ptr<Creature>>(e);
        d = td;
      }
    }
    return target;
  }

  std::optional<LightSpec> getGlow() override {
    std::optional<LightSpec> result = std::nullopt;
    auto effects = getEffects(AttributeType::VISIBILITY_DISTANCE);
    for (auto e : effects) {
      if (auto vm = std::dynamic_pointer_cast<VisibilityModifier>(e);
          vm && vm->glow) {
        if (!result || vm->light.distance > (*result).distance) {
          result = vm->light;
        }
      }
    }
    return result;
  }

  Effects getEffects(AttributeType type) {
    Effects effects;
    effects.insert(effects.end(), activeEffects.begin(), activeEffects.end());
    for (auto s : equipment->slots) {
      if (s->item == nullptr ||
          std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                    s->item->type.wearableType) == s->acceptTypes.end()) {
        continue;
      }
      for (auto e : s->item->effects) {
        if (e->type != type)
          continue;
        effects.push_back(e);
      }
    }
    return effects;
  }

  void prepareAiState();

  float intelligence = 1.f;
  float strength = 1.f;

  Attribute HP = Attribute(AttributeType::HP);
  Attribute HP_MAX = Attribute(AttributeType::HP_MAX);
  Attribute MP = Attribute(AttributeType::MP);
  Attribute MP_MAX = Attribute(AttributeType::MP_MAX);

  Attribute SPEED = Attribute(AttributeType::SPEED);
  Attribute VISIBILITY_DISTANCE = Attribute(AttributeType::VISIBILITY_DISTANCE);
  Attribute DMG = Attribute(AttributeType::DAMAGE);
  Attribute DEF = Attribute(AttributeType::DEFENSE);
  Attribute CRIT = Attribute(AttributeType::CRIT_CHANCE);

  Attribute INTELLIGENCE = Attribute(AttributeType::INTELLIGENCE);
  Attribute STRENGTH = Attribute(AttributeType::STRENGTH);

  float getThrowRange() { return STRENGTH(this) * throw_distance; }

  void applyEoT(EoT, int);

  std::vector<std::shared_ptr<Cell>> path;
  micropather::MicroPather *pather = nullptr;
  std::shared_ptr<Creature> lastTarget;
  std::shared_ptr<Cell> lastTargetCell;
  std::vector<std::shared_ptr<Cell>> findPath(std::shared_ptr<Cell>);

  AiState getAiState(std::shared_ptr<Object>);
  AiState lastAiState;

  void commit(std::string reason, int ap, bool s = false);
  int waitedAP = 0;

  void heal(int min, int max) {
    auto heal = R::Z(HP_MAX(this) / 100 * min,
                    HP_MAX(this) / 100 * max);
    hp += heal;
    if (HP(this) > HP_MAX(this)) {
      hp = HP_MAX(this);
    }
    commit("heal", 0);
    auto a = std::make_shared<ColorAnimation>(shared_from_this(), Color::fromHexString("#2222ff"), 8, true);
    AnimationEvent ae(a);
    eb::EventBus::FireEvent(ae);
  }

  void restoreMana(int min, int max) {
    auto heal = R::Z(MP_MAX(this) / 100 * min,
                    MP_MAX(this) / 100 * max);
    mp += heal;
    if (MP(this) > MP_MAX(this)) {
      mp = MP_MAX(this);
    }
    commit("manaRestore", 0);
    auto a = std::make_shared<ColorAnimation>(shared_from_this(), Color::fromHexString("#2222ff"), 8, true);
    AnimationEvent ae(a);
    eb::EventBus::FireEvent(ae);
  }

private:
  std::shared_ptr<Cell> cachedCell;
};

#endif // __CREATURE_H_
