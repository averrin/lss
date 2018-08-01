#ifndef __ANIMATION_H_
#define __ANIMATION_H_
#include "lss/game/object.hpp"
#include "lss/game/cell.hpp"
#include <functional>

typedef std::function<void(int)> FrameCallback;
typedef std::function<void()> AnimationCallback;

class Animation {
public:
    Animation(int s): steps(s) {}
    int counter = 0;
    int steps = 0;
    bool stopped = false;
    bool endless = false;
    FrameCallback frameCallback = nullptr;
    AnimationCallback animationCallback = nullptr;
    virtual int tick() = 0;
};

class MoveAnimation: public Animation {
public:
  MoveAnimation(std::shared_ptr<Object> o, std::vector<std::shared_ptr<Cell>> c, int s): Animation(s), object(o), path(c) {}
  std::shared_ptr<Object> object;
  std::vector<std::shared_ptr<Cell>> path;
  int tick();
};


#endif // __ANIMATION_H_
