#ifndef __OBJECT_H_
#define __OBJECT_H_

#include "EventHandler.hpp"
#include "EventRegisttration.hpp"
#include "Object.hpp"

#include "lss/game/cell.hpp"
#include "lss/game/light.hpp"

#include <fmt/format.h>

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

  LightSpec light;

  virtual std::optional<LightSpec> getGlow() { return std::nullopt; };

  int apLeft = -1;
  bool destructable = true;

  std::vector<eb::HandlerRegistrationPtr> handlers;
  std::shared_ptr<Cell> currentCell;

  void removeCell() {
    fmt::print("somebody remove cell from {}\n", name);
    currentCell = nullptr;
  }
  void setCurrentCell(std::shared_ptr<Cell> c) {
    if (c == nullptr) {
      throw std::runtime_error(fmt::format(
          "you trying to set nullptr as current cell for {}", name));
    }
    currentCell = c;
  }

  virtual bool interact(std::shared_ptr<Object> actor) { return false; };
};

typedef std::vector<std::shared_ptr<Object>> Objects;

#endif // __OBJECT_H_
