#ifndef __EFFECT_H_
#define __EFFECT_H_
#include <functional>
#include <memory>
#include <variant>
#include <vector>

#include "lss/game/attribute.hpp"
#include "lss/game/damageSpec.hpp"
#include "lss/game/overtime.hpp"
#include "lss/game/randomTools.hpp"

// TODO: refactor.
// maybe create variant with int, float, rndInt and rndFloat
// and write one function for baase modification (see lss/creature.cpp)
// and maybe make it Objects for castObjects filtering;

// TODO: area effects. With visualization
class Effect {
public:
  Effect(AttributeType at) : type(at){};
  Effect(AttributeType at, R::rndInt m) : type(at), modifier(R::I(m)){};
  Effect(AttributeType at, bool s) : type(at), special(s) {}
  Effect(AttributeType at, bool s, R::rndInt m)
      : type(at), special(s), modifier(m) {}
  virtual std::string getTitle() = 0;
  virtual std::string getSign() = 0;
  virtual std::shared_ptr<Effect> clone() = 0;
  virtual std::variant<float, int> getModifier() = 0;
  bool special = false;
  AttributeType type;
  R::rndInt modifier;

  void roll() {
    if (auto m = std::get_if<std::shared_ptr<R::Int>>(&modifier)) {
      modifier = R::I(modifier);
    }
  }
};

class SpeedModifier : public Effect {
public:
  SpeedModifier(float m) : Effect(AttributeType::SPEED), smodifier(m){};
  std::string getTitle();
  std::string getSign();
  float smodifier;
  std::shared_ptr<Effect> clone() {
    return std::make_shared<SpeedModifier>(smodifier);
  };
  std::variant<float, int> getModifier() { return smodifier; };
};

class HPModifier : public Effect {
public:
  HPModifier(R::rndInt m) : Effect(AttributeType::HP_MAX, m){};
  std::string getTitle();
  std::string getSign();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<HPModifier>(modifier);
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class VisibilityModifier : public Effect {
public:
  VisibilityModifier(float m)
      : Effect(AttributeType::VISIBILITY_DISTANCE), vmodifier(m){};
  std::string getTitle();
  std::string getSign();
  float vmodifier;
  std::variant<float, int> getModifier() { return vmodifier; };

  std::shared_ptr<Effect> clone() {
    return std::make_shared<VisibilityModifier>(vmodifier);
  };
};

class IntelligenceModifier : public Effect {
public:
  IntelligenceModifier(float m)
      : Effect(AttributeType::INTELLIGENCE), vmodifier(m){};
  std::string getTitle();
  std::string getSign();
  float vmodifier;
  std::variant<float, int> getModifier() { return vmodifier; };

  std::shared_ptr<Effect> clone() {
    return std::make_shared<IntelligenceModifier>(vmodifier);
  };
};

class StrengthModifier : public Effect {
public:
  StrengthModifier(float m) : Effect(AttributeType::STRENGTH), vmodifier(m){};
  std::string getTitle();
  std::string getSign();
  float vmodifier;
  std::variant<float, int> getModifier() { return vmodifier; };

  std::shared_ptr<Effect> clone() {
    return std::make_shared<StrengthModifier>(vmodifier);
  };
};

class CritModifier : public Effect {
public:
  CritModifier(float m) : Effect(AttributeType::CRIT_CHANCE), cmodifier(m){};
  std::string getTitle();
  std::string getSign();
  float cmodifier;
  std::shared_ptr<Effect> clone() {
    return std::make_shared<CritModifier>(cmodifier);
  };
  std::variant<float, int> getModifier() { return cmodifier; };
};

class ArmorValue : public Effect {
public:
  ArmorValue(R::rndInt m) : Effect(AttributeType::DEFENSE, true, m){};
  std::string getTitle();
  std::string getSign();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<ArmorValue>(modifier);
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class SpecialPostfix : public Effect {
public:
  SpecialPostfix(std::string n) : Effect(AttributeType::NONE, true), name(n){};
  std::string getTitle() { return name; };
  std::string getSign() { return ""; };
  std::string name;
  std::shared_ptr<Effect> clone() {
    return std::make_shared<SpecialPostfix>(name);
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class SpecialPrefix : public Effect {
public:
  SpecialPrefix(std::string n) : Effect(AttributeType::NONE, true), name(n){};
  std::string getTitle() { return name; };
  std::string getSign() { return ""; };
  std::string name;
  std::shared_ptr<Effect> clone() {
    return std::make_shared<SpecialPrefix>(name);
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class MeleeDamage : public Effect {
public:
  MeleeDamage(DamageSpec spec)
      : Effect(AttributeType::NONE, true), dmgSpec(spec){};
  std::string getTitle();
  std::string getSign();
  DamageSpec dmgSpec;
  std::shared_ptr<Effect> clone() {
    return std::make_shared<MeleeDamage>(dmgSpec.roll());
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class TraitEffect : public Effect {
public:
  TraitEffect(Trait t) : Effect(AttributeType::TRAITS), trait(t){};
  Trait trait;
  std::string getTitle();
  std::string getSign();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<TraitEffect>(trait);
  };
  std::variant<float, int> getModifier() { return 0; };
};

class LastingEffect : public Effect {
public:
  LastingEffect(std::shared_ptr<Effect> e, int d)
      : Effect(e->type), effect(e), duration(d), currentDuration(d){};
  std::shared_ptr<Effect> effect;
  int duration;
  int currentDuration;
  std::string getTitle();
  std::string getSign();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<LastingEffect>(effect, duration);
  };
  std::variant<float, int> getModifier() { return effect->getModifier(); };
};

class OnHitEffect : public Effect {
public:
  OnHitEffect(std::shared_ptr<Effect> e, float p)
      : Effect(e->type), effect(e), probability(p){};
  std::shared_ptr<Effect> effect;
  float probability;
  std::string getTitle();
  std::string getSign();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<OnHitEffect>(effect, probability);
  };
  std::variant<float, int> getModifier() { return effect->getModifier(); };
};

class OverTimeEffect;
typedef std::function<void(std::shared_ptr<Creature>)> EffectApplier;

class OverTimeEffect : public Effect {
public:
  OverTimeEffect(std::string n, R::rndInt m, int ti, EoT t)
      : Effect(AttributeType::NONE, m), name(n), tick(ti), type(t){};
  std::string name;
  int tick;
  int accomulator = 0;
  // EffectApplier applier;
  EoT type;
  std::string getTitle();
  std::string getSign();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<OverTimeEffect>(name, modifier, tick, type);
  };

  std::variant<float, int> getModifier() { return R::get(modifier); };
  int getModifier(int ap) {
    auto result = 0;
    auto m = std::get<int>(getModifier());

    accomulator += ap;
    while (accomulator >= tick) {
      accomulator -= tick;
      result += m;
    }
    return m;
  }
};

namespace OverTimeEffects {
const auto HEAL = std::make_shared<OverTimeEffect>("Heal", 5, 500, EoT::HEAL);
const auto REGEN =
    std::make_shared<OverTimeEffect>("Heal", 1, 50000, EoT::HEAL);
const auto MANA_RESTORE_LESSER = std::make_shared<OverTimeEffect>(
    "Mana restore", 1, 100000, EoT::MANA_RESTORE);
const auto MANA_RESTORE = std::make_shared<OverTimeEffect>(
    "Mana restore", 1, 60000, EoT::MANA_RESTORE);
const auto POISON =
    std::make_shared<OverTimeEffect>("Poison", 3, 500, EoT::POISON);
} // namespace OverTimeEffects

typedef std::vector<std::shared_ptr<Effect>> Effects;

#endif
