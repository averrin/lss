#include <memory>

#include "lss/game/door.hpp"
#include "EventBus.hpp"

Door::Door() {
    passThrough = false;
    seeThrough = false;
};

bool Door::interact() {
    if (!opened) {
        opened = true;
        passThrough = true;
        seeThrough = true;
        DoorOpenedEvent e(shared_from_this());
		eb::EventBus::FireEvent(e);
        return true;
    }
    return false;
}

DoorOpenedEvent::DoorOpenedEvent(eb::ObjectPtr s): eb::Event(s) {}
