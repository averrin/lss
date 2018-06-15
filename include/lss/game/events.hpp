#ifndef __EVENTS_H_
#define __EVENTS_H_
#include "Event.hpp"

class Object;
class Item;
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

class ItemTakenEvent: public eb::Event {
public:
    ItemTakenEvent(eb::ObjectPtr, std::shared_ptr<Item>);
    std::shared_ptr<Item> item;
};

#endif // __EVENTS_H_
