#ifndef __DOOR_H_
#define __DOOR_H_
#include "lss/game/object.hpp"

#include "Event.hpp"

class Door : public Object {
public:
    // Door(bool o = false, bool l = false) : Object(), opened(o), locked(l) {}
    Door();
    bool interact();

    bool opened;
    bool locked;
};

class DoorOpenedEvent: public eb::Event {
public:
    DoorOpenedEvent(eb::ObjectPtr);
};

#endif // __DOOR_H_
