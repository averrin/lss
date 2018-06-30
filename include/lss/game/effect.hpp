#ifndef __EFFECT_H_
#define __EFFECT_H_
#include <memory>
#include <vector>

#include "lss/game/attribute.hpp"

#include "fmt/format.h"

class Player;
class Effect {
public:
  Effect(AttributeType at, float m) : type(at), modifier(m){};
  Effect(AttributeType at, bool s) : type(at), special(s) {}
  Effect(AttributeType at, bool s, float m)
      : type(at), special(s), modifier(m) {}
  virtual void apply(Player *) = 0;
  virtual void undo(Player *) = 0;
  virtual std::string getTitle() = 0;
  bool special = false;
  AttributeType type;
  float modifier;
};

class SpeedModifier : public Effect {
public:
  SpeedModifier(float m) : Effect(AttributeType::SPEED, m){};
  void apply(Player *);
  void undo(Player *);
  std::string getTitle();
};

class HPModifier : public Effect {
public:
  HPModifier(float m) : Effect(AttributeType::HP_MAX, m){};
  void apply(Player *);
  void undo(Player *);
  std::string getTitle();
};

class VisibilityModifier : public Effect {
public:
  VisibilityModifier(float m) : Effect(AttributeType::VISIBILITY_DISTANCE, m){};
  void apply(Player *);
  void undo(Player *);
  std::string getTitle();
};

class SpecialPostfix : public Effect {
public:
  SpecialPostfix(std::string n) : Effect(AttributeType::NONE, true), name(n){};
  void apply(Player *){};
  void undo(Player *){};
  std::string getTitle() { return name; };
  std::string name;
};

class SpecialPrefix : public Effect {
public:
  SpecialPrefix(std::string n) : Effect(AttributeType::NONE, true), name(n){};
  void apply(Player *){};
  void undo(Player *){};
  std::string getTitle() { return name; };
  std::string name;
};

class MeleeDamage : public Effect {
public:
  MeleeDamage(int d, int e, int m)
      : Effect(AttributeType::NONE, m, true), dices(d), edges(e){};
  void apply(Player *);
  void undo(Player *);
  std::string getTitle();
  int dices;
  int edges;
  int modifier;
};

typedef std::vector<std::shared_ptr<Effect>> Effects;

#endif
