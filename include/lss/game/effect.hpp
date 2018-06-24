#ifndef __EFFECT_H_
#define __EFFECT_H_
#include <memory>
#include <vector>

class Player;
class Effect {
public:
  Effect(){};
  Effect(bool s) : special(s) {}
  virtual void apply(Player *) = 0;
  virtual void undo(Player *) = 0;
  virtual std::string getTitle() = 0;
  bool special = false;
};

class SpeedModifier : public Effect {
public:
  SpeedModifier(float m) : Effect(), modifier(m){};
  void apply(Player *);
  void undo(Player *);
  std::string getTitle();
  float modifier;
};

class HPModifier : public Effect {
public:
  HPModifier(float m) : Effect(), modifier(m){};
  void apply(Player *);
  void undo(Player *);
  std::string getTitle();
  float modifier;
};

class SpecialPostfix : public Effect {
public:
  SpecialPostfix(std::string n) : Effect(true), name(n){};
  void apply(Player *){};
  void undo(Player *){};
  std::string getTitle() {return name;};
  std::string name;
};

class SpecialPrefix : public Effect {
public:
  SpecialPrefix(std::string n) : Effect(true), name(n){};
  void apply(Player *){};
  void undo(Player *){};
  std::string getTitle() {return name;};
  std::string name;
};

class MeleeDamage : public Effect {
public:
  MeleeDamage(int d, int e, int m) : Effect(true), modifier(m), dices(d), edges(e){};
  void apply(Player *);
  void undo(Player *);
  std::string getTitle();
  int dices;
  int edges;
  int modifier;
};

typedef std::vector<std::shared_ptr<Effect>> Effects;

#endif
