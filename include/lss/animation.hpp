#ifndef __ANIMATION_H_
#define __ANIMATION_H_
#include "lss/color.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/object.hpp"
#include <functional>

typedef std::function<void(int)> FrameCallback;
typedef std::function<void()> AnimationCallback;

class Fragment;
class Animation {
public:
  Animation(int s) : steps(s) {}
  int counter = 0;
  int steps = 0;
  bool stopped = false;
  bool endless = false;
  FrameCallback frameCallback = nullptr;
  AnimationCallback animationCallback = nullptr;
  virtual std::vector<std::shared_ptr<Cell>> tick() = 0;
};

class MoveAnimation : public Animation {
public:
  MoveAnimation(std::shared_ptr<Object> o, std::vector<std::shared_ptr<Cell>> c,
                int s)
      : Animation(s), object(o), path(c) {}
  std::shared_ptr<Object> object;
  std::vector<std::shared_ptr<Cell>> path;
  std::vector<std::shared_ptr<Cell>> tick();
};

class ColorAnimation : public Animation {
public:
  ColorAnimation(std::shared_ptr<Object> o, Color c, int s, bool p, bool b = false)
      : Animation(s), object(o), cell(o->currentCell), targetColor(c), pulse(p), bg(b) {}

  ColorAnimation(std::shared_ptr<Cell> _cell, Color c, int s, bool p, bool b = false)
      : Animation(s), cell(_cell), targetColor(c), pulse(p), bg(b) {}
  std::shared_ptr<Cell> cell;
  std::shared_ptr<Object> object = nullptr;
  Color targetColor;
  Color initColor;
  std::shared_ptr<Fragment> fragment;
  bool pulse;
  bool wayback = false;
  bool bg = false;
  std::vector<std::shared_ptr<Cell>> tick();
};

#endif // __ANIMATION_H_
