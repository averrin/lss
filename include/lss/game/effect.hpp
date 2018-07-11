#ifndef __EFFECT_H_
#define __EFFECT_H_
#include <memory>
#include <variant>
#include <vector>

#include "lss/game/attribute.hpp"
#include "lss/game/randomTools.hpp"

#include "fmt/format.h"

class Player;
class Effect {
public:
  Effect(AttributeType at, R::rndFloat m) : type(at), modifier(R::F(m)){};
  Effect(AttributeType at, bool s) : type(at), special(s) {}
  Effect(AttributeType at, bool s, R::rndFloat m)
      : type(at), special(s), modifier(m) {}
  virtual std::string getTitle() = 0;
  virtual std::shared_ptr<Effect> clone() = 0;
  bool special = false;
  AttributeType type;
  R::rndFloat modifier;

  void roll() {
    if (auto m = std::get_if<std::shared_ptr<R::Float>>(&modifier)) {
      modifier = R::F(modifier);
    }
  }
};

class SpeedModifier : public Effect {
public:
  SpeedModifier(R::rndFloat m) : Effect(AttributeType::SPEED, m){};
  std::string getTitle();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<SpeedModifier>(modifier);
  };
};

class HPModifier : public Effect {
public:
  HPModifier(R::rndFloat m) : Effect(AttributeType::HP_MAX, m){};
  std::string getTitle();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<HPModifier>(modifier);
  };
};

class VisibilityModifier : public Effect {
public:
  VisibilityModifier(R::rndFloat m)
      : Effect(AttributeType::VISIBILITY_DISTANCE, m){};
  std::string getTitle();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<VisibilityModifier>(modifier);
  };
};

class ArmorValue : public Effect {
public:
  ArmorValue(R::rndFloat m) : Effect(AttributeType::DEFENSE, true, m){};
  std::string getTitle();
  std::shared_ptr<Effect> clone() {
    return std::make_shared<ArmorValue>(modifier);
  };
};

class SpecialPostfix : public Effect {
public:
  SpecialPostfix(std::string n) : Effect(AttributeType::NONE, true), name(n){};
  std::string getTitle() { return name; };
  std::string name;
  std::shared_ptr<Effect> clone() {
    return std::make_shared<SpecialPostfix>(name);
  };
};

class SpecialPrefix : public Effect {
public:
  SpecialPrefix(std::string n) : Effect(AttributeType::NONE, true), name(n){};
  std::string getTitle() { return name; };
  std::string name;
  std::shared_ptr<Effect> clone() {
    return std::make_shared<SpecialPrefix>(name);
  };
};

class MeleeDamage : public Effect {
public:
  MeleeDamage(R::rndFloat m, R::rndInt d, R::rndInt e)
      : Effect(AttributeType::NONE, true, m), dices(R::I(d)), edges(R::I(e)){};
  std::string getTitle();
  R::rndInt dices;
  R::rndInt edges;
  std::shared_ptr<Effect> clone() {
    return std::make_shared<MeleeDamage>(modifier, dices, edges);
  };
};

typedef std::vector<std::shared_ptr<Effect>> Effects;

#endif
