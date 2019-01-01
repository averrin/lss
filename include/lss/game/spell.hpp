#ifndef __SPELL_H_
#define __SPELL_H_
#include "lss/game/damage.hpp"
#include "lss/game/effect.hpp"
#include "lss/game/location.hpp"
#include "lss/game/object.hpp"
#include "lss/game/terrain.hpp"

class Spell : public Object {
public:
  Spell(std::string n, int l = 0, int c = 0, int a = 0)
      : Object(), name(n), level(l), cost(c), ap(a) {}
  std::string name;
  int level;
  int cost;
  int ap;
  friend bool operator!=(Spell &lhs, const Spell &rhs) {
    return lhs.name != rhs.name;
  }
  friend bool operator==(Spell &lhs, const Spell &rhs) {
    return lhs.name == rhs.name;
  }
};

class ToggleTraitSpell : public Spell {
public:
  ToggleTraitSpell(std::string n, Trait t) : Spell(n), trait(t) {}
  ToggleTraitSpell(std::string n, Trait t, float p)
      : Spell(n), trait(t), probability(p) {}
  float probability = 1;
  Trait trait;
};

class EffectSpell : public Spell {
public:
  EffectSpell(std::string n, std::shared_ptr<Effect> e) : Spell(n), effect(e) {}
  EffectSpell(std::string n, int l, std::shared_ptr<Effect> e)
      : Spell(n, l), effect(e) {}
  std::shared_ptr<Effect> effect;
};

class CellSpell : public Spell {
public:
  CellSpell(std::string n, TerrainSpec ts) : Spell(n), spec(ts) {}
  CellSpell(std::string n, int l, TerrainSpec ts) : Spell(n, l), spec(ts) {}
  TerrainSpec spec;
  void applyEffect(std::shared_ptr<Location> location,
                   std::shared_ptr<Cell> c) {

    if (spec == TerrainType::FROSTBALL) {
      auto fb = std::make_shared<Terrain>(spec, 8);
      fb->setCurrentCell(c);
      location->addObject(fb);
      c->addFeature(CellFeature::FROST);
      for (auto n : location->getNeighbors(c)) {
        n->addFeature(CellFeature::FROST);
      }
    } else if (spec == TerrainType::FIREBALL) {
      auto fb = std::make_shared<Terrain>(spec, 8);
      fb->setCurrentCell(c);
      location->addObject(fb);
      c->removeFeature(CellFeature::FROST);
      for (auto n : location->getNeighbors(c)) {
        n->removeFeature(CellFeature::FROST);
      }
    } else if (spec == TerrainType::ACIDPOOL) {
      auto fb = std::make_shared<Terrain>(spec, -1);
      fb->setCurrentCell(c);
      location->addObject(fb);
      c->addFeature(CellFeature::ACID);
    }

    location->invalidate("apply spell effect");
  }
  virtual void applySpell(std::shared_ptr<Creature> caster,
                          std::shared_ptr<Location> location,
                          std::shared_ptr<Cell> c) {
    applyEffect(location, c);
  }
};

class DamageSpell : public CellSpell {
public:
  DamageSpell(std::string n, DamageSpec dmg, TerrainSpec ts, bool d = false)
      : CellSpell(n, ts), damage(dmg), destroyObjects(d) {}
  DamageSpell(std::string n, int l, DamageSpec dmg, TerrainSpec ts,
              bool d = false)
      : CellSpell(n, l, ts), damage(dmg), destroyObjects(d) {}
  DamageSpec damage;
  bool destroyObjects = false;
  void applySpell(std::shared_ptr<Creature> caster, std::shared_ptr<Location>,
                  std::shared_ptr<Cell>);
};

class DrillSpell : public DamageSpell {
public:
  DrillSpell(std::string n)
      : DamageSpell(n, DamageSpec(0, 0, 0, DamageType::BASIC),
                    TerrainType::DRILL, true) {}
  void applySpell(std::shared_ptr<Creature> caster, std::shared_ptr<Location>,
                  std::shared_ptr<Cell>);
};

class RadiusSpell : public Spell {
public:
  RadiusSpell(std::string n, int l, std::shared_ptr<Spell> s, float r, int c)
      : Spell(n, l, c), spell(s), radius(r) {}
  std::shared_ptr<Spell> spell;
  float radius;
};

class LineSpell : public Spell {
public:
  LineSpell(std::string n, int l, std::shared_ptr<Spell> s, int le, int c)
      : Spell(n, l, c), spell(s), length(le) {}
  std::shared_ptr<Spell> spell;
  int length;
};

class TargetSpell : public Spell {
public:
  TargetSpell(std::string n, int l, std::shared_ptr<Spell> s, int le, int c)
      : Spell(n, l, c), spell(s), length(le) {}
  std::shared_ptr<Spell> spell;
  int length;
};

#endif // __SPELL_H_
