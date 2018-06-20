#ifndef __OBJECT_H_
#define __OBJECT_H_

#include "Object.hpp"

#include "lss/game/cell.hpp"
#include "lss/game/location.hpp"

class Object : public eb::Object, public std::enable_shared_from_this<Object> {
public:
  Object();
  // virtual ~Object() = default;
  bool passThrough = true;
  bool seeThrough = true;

  std::shared_ptr<Cell> currentCell;

  virtual bool interact(std::shared_ptr<Object> actor) { return false; };
};

typedef std::vector<std::shared_ptr<Object>> Objects;

#endif // __OBJECT_H_
