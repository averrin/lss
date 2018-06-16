#ifndef __EVENTS_H_
#define __EVENTS_H_

#include <lss/game/direction.hpp>

#include "Event.hpp"


class Object;
typedef std::vector<std::shared_ptr<Object>> Objects;

class Item;
class Cell;
class DoorOpenedEvent : public eb::Event {
public:
  DoorOpenedEvent(eb::ObjectPtr);
};

class EnemyTakeDamageEvent : public eb::Event {
public:
  EnemyTakeDamageEvent(eb::ObjectPtr, int);
  int damage;
};

class EnemyDiedEvent : public eb::Event {
public:
  EnemyDiedEvent(eb::ObjectPtr);
};

class ItemTakenEvent : public eb::Event {
public:
  ItemTakenEvent(eb::ObjectPtr, std::shared_ptr<Item>);
  std::shared_ptr<Item> item;
};


class LeaveCellEvent : public eb::Event {
public:
  LeaveCellEvent(eb::ObjectPtr, std::shared_ptr<Cell>);
  std::shared_ptr<Cell> cell;
};

class EnterCellEvent : public eb::Event {
public:
  EnterCellEvent(eb::ObjectPtr, std::shared_ptr<Cell>);
  std::shared_ptr<Cell> cell;
};

class ItemsFoundEvent : public eb::Event {
public:
  ItemsFoundEvent(eb::ObjectPtr, Objects);
  Objects items;
};


/**********/

class CommandEvent: public eb::Event {
public:
  CommandEvent(eb::ObjectPtr);
};

class MoveCommandEvent: public CommandEvent {
public:
    MoveCommandEvent(Direction);
    Direction direction;
};

#endif // __EVENTS_H_
