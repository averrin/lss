#include <memory>

#include "EventBus.hpp"
#include "lss/game/door.hpp"
#include "lss/game/events.hpp"

Door::Door() : Object("door") {
  zIndex = 1;
  passThrough = false;
  seeThrough = false;
};

bool Door::interact(std::shared_ptr<Object> actor) {
  if (!opened) {
    opened = true;
    passThrough = true;
    seeThrough = true;
    currentCell->seeThrough = true;
    currentCell->invalidate("door open");
    DoorOpenedEvent e(shared_from_this());
    eb::EventBus::FireEvent(e);
    return true;
  }
  return false;
}

DoorOpenedEvent::DoorOpenedEvent(eb::ObjectPtr s) : eb::Event(s) {}
