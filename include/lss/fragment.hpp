#ifndef __FRAGMENT_H_
#define __FRAGMENT_H_

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>

#include "lss/color.hpp"
#include "lss/game/cell.hpp"
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
  std::string render(State *s, std::map<std::string, tpl_arg> = {});
  int index;
  int length;

  virtual ~Fragment();
  bool needRender = true;
  bool damaged = true;
  void invalidate() { damaged = true; }
  void invalidate(bool killCache) { damaged = true; if(killCache) cache = ""; }

  std::string cache;
  int alpha = Cell::DEFAULT_LIGHT;
  int bgAlpha = 0;
  std::string bgColor = COLORS::BG;
  std::string fgColor = COLORS::FG;

  void setAlpha(int a) {
    if (alpha != a) {
      alpha = a;
      invalidate();
    }
  }
  void setBgAlpha(int a) {
    if (bgAlpha != a) {
      bgAlpha = a;
      invalidate();
    }
  }
  void setBgColor(std::string c) {
    if (bgColor == c)
      return;
    bgColor = c;
    if (bgColor != COLORS::BG) {
      bgAlpha = 100;
    } else {
      bgAlpha = 0;
    }
    invalidate();
  }
  void setFgColor(std::string c) {
    if (fgColor == c)
      return;
    fgColor = c;
    invalidate();
  }

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
  HeroSign(std::shared_ptr<Cell>, std::string);
};

class EnemySign : public Fragment {
public:
  EnemySign(std::shared_ptr<Cell>, EnemySpec);
};

class DoorSign : public Fragment {
public:
  DoorSign(bool);
};

class ItemSign : public Fragment {
public:
  ItemSign(std::shared_ptr<Cell>, ItemSpec);
};

class TerrainSign : public Fragment {
public:
  TerrainSign(std::shared_ptr<Cell>, TerrainSpec);
};

typedef std::vector<std::shared_ptr<Fragment>> Fragments;

#endif // __FRAGMENT_H_
