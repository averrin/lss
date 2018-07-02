#ifndef __DOOR_H_
#define __DOOR_H_
#include "lss/game/object.hpp"

class Door : public Object {
public:
  // Door(bool o = false, bool l = false) : Object(), opened(o), locked(l) {}
  Door();
  bool interact(std::shared_ptr<Object>);

  bool opened = false;
  bool locked;
};

#endif // __DOOR_H_
