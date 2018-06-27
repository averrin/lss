#ifndef __EVENTS_H_
#define __EVENTS_H_

#include <lss/game/direction.hpp>

#include "Event.hpp"

class Object;
typedef std::vector<std::shared_ptr<Object>> Objects;

class Item;
class Slot;
class Cell;
class Spell;
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

class DropEvent : public eb::Event {
public:
  DropEvent(eb::ObjectPtr, std::shared_ptr<Item>);
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

class DigEvent : public eb::Event {
public:
  DigEvent(eb::ObjectPtr, std::shared_ptr<Cell>);
  std::shared_ptr<Cell> cell;
};

class CommitEvent : public eb::Event {
public:
  CommitEvent(eb::ObjectPtr, int);
  int actionPoints;
};

/**********/
class MessageEvent : public eb::Event {
public:
  MessageEvent(eb::ObjectPtr, std::string);
  std::string message;
};

/**********/

class CommandEvent : public eb::Event {
public:
  CommandEvent(): eb::Event(nullptr) {};
  CommandEvent(eb::ObjectPtr);
};

class MoveCommandEvent : public CommandEvent {
public:
    MoveCommandEvent() : CommandEvent(nullptr) {}
  MoveCommandEvent(Direction);
  Direction direction;
};

class QuitCommandEvent : public CommandEvent {
public:
  QuitCommandEvent();
};

class PickCommandEvent : public CommandEvent {
public:
  PickCommandEvent();
};

class DigCommandEvent : public CommandEvent {
public:
    DigCommandEvent() : CommandEvent(nullptr) {}
  DigCommandEvent(Direction);
  Direction direction;
};

class WalkCommandEvent : public CommandEvent {
public:
  WalkCommandEvent() : CommandEvent(nullptr) {}
  WalkCommandEvent(Direction);
  Direction direction;
};

class AttackCommandEvent : public CommandEvent {
public:
  AttackCommandEvent() : CommandEvent(nullptr) {}
  AttackCommandEvent(Direction);
  Direction direction;
};

class EquipCommandEvent : public CommandEvent {
public:
  EquipCommandEvent(std::shared_ptr<Slot>, std::shared_ptr<Item>);
  EquipCommandEvent(std::shared_ptr<Slot>);
  EquipCommandEvent();
  std::shared_ptr<Slot> slot;
  std::shared_ptr<Item> item;
};

class UnEquipCommandEvent : public CommandEvent {
public:
  UnEquipCommandEvent(std::shared_ptr<Slot>);
  std::shared_ptr<Slot> slot;
};

class HelpCommandEvent : public CommandEvent {
public:
  HelpCommandEvent();
};

class InventoryCommandEvent : public CommandEvent {
public:
  InventoryCommandEvent();
};

class DropCommandEvent : public CommandEvent {
public:
  DropCommandEvent(std::shared_ptr<Item>);
  DropCommandEvent();
  std::shared_ptr<Item> item;
};

class WaitCommandEvent : public CommandEvent {
public:
  WaitCommandEvent();
};

class ZapCommandEvent : public CommandEvent {
public:
  ZapCommandEvent(std::shared_ptr<Spell>);
  ZapCommandEvent();
  std::shared_ptr<Spell> spell;
};
#endif // __EVENTS_H_
