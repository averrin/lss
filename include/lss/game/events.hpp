#ifndef __EVENTS_H_
#define __EVENTS_H_
#include "Event.hpp"

class Object;
class DoorOpenedEvent: public eb::Event {
public:
    DoorOpenedEvent(eb::ObjectPtr);
};

class EnemyTakeDamageEvent: public eb::Event {
public:
    EnemyTakeDamageEvent(eb::ObjectPtr, int);
    int damage;
};

class EnemyDiedEvent: public eb::Event {
public:
    EnemyDiedEvent(eb::ObjectPtr);
};


#endif // __EVENTS_H_
