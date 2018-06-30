#ifndef __FRAGMENT_H_
#define __FRAGMENT_H_

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>

#include "CinderPango.h"
#include "lss/game/enemy.hpp"
#include "lss/game/item.hpp"

typedef std::variant<int, float, bool, std::string> tpl_arg;

class State;
struct LssEvent {
  virtual ~LssEvent(){};
};
class Fragment {
public:
  Fragment(std::string t, std::map<std::string, tpl_arg> args);
  Fragment(std::string t, bool needRender);
  Fragment(std::string t);
  std::string render(State *s);
  int index;
  int length;
  PangoRectangle rect;

  virtual ~Fragment();
  bool needRender = true;
  bool damaged = true;
  std::string cache;

protected:
  std::map<std::string, tpl_arg> args;
  std::string template_str;
};

class CellSign : public Fragment {
public:
  CellSign(CellType, bool, bool);
  void update(CellType, bool, bool);
};

class HeroSign : public Fragment {
public:
  HeroSign();
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

typedef std::vector<std::shared_ptr<Fragment>> Fragments;

#endif // __FRAGMENT_H_
