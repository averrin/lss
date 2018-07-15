#ifndef __FRAGMENT_H_
#define __FRAGMENT_H_

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>

// TODO: find why this unnecessary include is necessary
#include "CinderPango.h"

#include "lss/game/enemy.hpp"
#include "lss/game/item.hpp"
#include "lss/game/terrain.hpp"

typedef std::variant<int, float, bool, std::string> tpl_arg;

class State;
struct LssEvent {
  virtual ~LssEvent(){};
};
class Fragment {
public:
  Fragment(std::string t, std::map<std::string, tpl_arg> args);
  Fragment(std::string t, std::map<std::string, tpl_arg>, bool needRender);
  Fragment(std::string t, bool needRender);
  Fragment(std::string t);
  std::string render(State *s);
  int index;
  int length;

  virtual ~Fragment();
  bool needRender = true;
  bool damaged = true;
  std::string cache;

  friend bool operator==(Fragment &t1, const Fragment &t2) {
    return t1.template_str == t2.template_str;
  }

protected:
  std::map<std::string, tpl_arg> args;
  std::string template_str;
};

class CellSign : public Fragment {
public:
  CellSign(std::shared_ptr<Cell>);
};

class HeroSign : public Fragment {
public:
  HeroSign(std::string);
};

class EnemySign : public Fragment {
public:
  EnemySign(EnemySpec);
};

class DoorSign : public Fragment {
public:
  DoorSign(bool);
};

class ItemSign : public Fragment {
public:
  ItemSign(ItemSpec);
};

class TerrainSign : public Fragment {
public:
  TerrainSign(TerrainSpec);
};

typedef std::vector<std::shared_ptr<Fragment>> Fragments;

#endif // __FRAGMENT_H_
