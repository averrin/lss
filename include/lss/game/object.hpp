#ifndef __OBJECT_H_
#define __OBJECT_H_

#include "EventHandler.hpp"
#include "EventRegisttration.hpp"
#include "Object.hpp"

#include "lss/game/cell.hpp"

class Object : public eb::Object, public std::enable_shared_from_this<Object> {
public:
  Object() {}
  Object(std::string n) : name(n) {}
  ~Object() { clearHandlers(); }
  void clearHandlers() {
    for (auto r : handlers) {
      r->removeHandler();
    }
    handlers.clear();
  }
  // virtual ~Object() = default;
  bool passThrough = true;
  bool seeThrough = true;
  std::string name;
  int zIndex = 0;

  std::vector<eb::HandlerRegistrationPtr> handlers;
  std::shared_ptr<Cell> currentCell;

  virtual bool interact(std::shared_ptr<Object> actor) { return false; };
};

typedef std::vector<std::shared_ptr<Object>> Objects;

#endif // __OBJECT_H_
