#ifndef __EFFECT_H_
#define __EFFECT_H_
#include <memory>
#include <variant>
#include <vector>
#include <functional>

#include "lss/game/attribute.hpp"
#include "lss/game/randomTools.hpp"

// TODO: refactor.
// maybe create variant with int, float, rndInt and rndFloat
// and write one function for baase modification (see lss/creature.cpp)
// and maybe make it Objects for castObjects filtering;
class Player;
class Effect {
public:
  Effect(AttributeType at) : type(at){};
  Effect(AttributeType at, R::rndInt m) : type(at), modifier(R::I(m)){};
  Effect(AttributeType at, bool s) : type(at), special(s) {}
  Effect(AttributeType at, bool s, R::rndInt m)
      : type(at), special(s), modifier(m) {}
  virtual std::string getTitle() = 0;
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
  SpeedModifier(R::rndInt m) : Effect(AttributeType::SPEED, m){};
  std::string getTitle();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<SpeedModifier>(modifier);
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class HPModifier : public Effect {
public:
  HPModifier(R::rndInt m) : Effect(AttributeType::HP_MAX, m){};
  std::string getTitle();
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
  float vmodifier;
  std::variant<float, int> getModifier() { return vmodifier; };

  std::shared_ptr<Effect> clone() {
    return std::make_shared<VisibilityModifier>(vmodifier);
  };
};

class CritModifier : public Effect {
public:
  CritModifier(R::rndInt m) : Effect(AttributeType::CRIT_CHANCE, m){};
  std::string getTitle();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<CritModifier>(modifier);
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class ArmorValue : public Effect {
public:
  ArmorValue(R::rndInt m) : Effect(AttributeType::DEFENSE, true, m){};
  std::string getTitle();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<ArmorValue>(modifier);
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class SpecialPostfix : public Effect {
public:
  SpecialPostfix(std::string n) : Effect(AttributeType::NONE, true), name(n){};
  std::string getTitle() { return name; };
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
  std::string name;
  std::shared_ptr<Effect> clone() {
    return std::make_shared<SpecialPrefix>(name);
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class MeleeDamage : public Effect {
public:
  MeleeDamage(R::rndInt m, R::rndInt d, R::rndInt e)
      : Effect(AttributeType::NONE, true, m), dices(R::I(d)), edges(R::I(e)){};
  std::string getTitle();
  R::rndInt dices;
  R::rndInt edges;
  std::shared_ptr<Effect> clone() {
    return std::make_shared<MeleeDamage>(modifier, dices, edges);
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class Poison : public Effect {
public:
  Poison(int dmg, int d)
      : Effect(AttributeType::NONE), damage(dmg), duration(d){};
  int damage;
  int duration;
  std::string getTitle();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<Poison>(damage, duration);
  };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class Vampire : public Effect {
public:
  Vampire(int dmg) : Effect(AttributeType::NONE), damage(dmg){};
  int damage;
  std::string getTitle();
  std::shared_ptr<Effect> clone() { return std::make_shared<Vampire>(damage); };
  std::variant<float, int> getModifier() { return R::get(modifier); };
};

class TraitEffect : public Effect {
public:
  TraitEffect(Trait t) : Effect(AttributeType::TRAITS), trait(t){};
  Trait trait;
  std::string getTitle();
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
  std::shared_ptr<Effect> clone() {
    return std::make_shared<LastingEffect>(effect, duration);
  };
  std::variant<float, int> getModifier() { return effect->getModifier(); };
};

class OverTimeEffect;
typedef std::function<void(std::shared_ptr<Creature>)> EffectApplier;

class OverTimeEffect : public Effect {
public:
  OverTimeEffect(int t, EffectApplier ea)
      : Effect(AttributeType::NONE), tick(t), applier(ea){};
    int tick;
    int accomulator = 0;
    EffectApplier applier;
  std::string getTitle();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<OverTimeEffect>(tick, applier);
  };
  std::variant<float, int> getModifier() { return 0; };

  void apply(std::shared_ptr<Creature> c, int ap);
};

typedef std::vector<std::shared_ptr<Effect>> Effects;

#endif
