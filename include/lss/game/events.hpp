#ifndef __EVENTS_H_
#define __EVENTS_H_

#include <functional>
#include <lss/game/direction.hpp>

#include "Event.hpp"

class Item;
typedef std::vector<std::shared_ptr<Item>> Items;
class Slot;
class Cell;
class Spell;
class Damage;
class Creature;
class DoorOpenedEvent : public eb::Event {
public:
  DoorOpenedEvent(eb::ObjectPtr);
};

class EnemyTakeDamageEvent : public eb::Event {
public:
  EnemyTakeDamageEvent(eb::ObjectPtr, std::shared_ptr<Damage>);
  std::shared_ptr<Damage> damage;
};

class HeroTakeDamageEvent : public eb::Event {
public:
  HeroTakeDamageEvent(eb::ObjectPtr, std::shared_ptr<Damage>);
  std::shared_ptr<Damage> damage;
};

class EnemyDiedEvent : public eb::Event {
public:
  EnemyDiedEvent(eb::ObjectPtr);
};

class HeroDiedEvent : public eb::Event {
public:
  HeroDiedEvent(eb::ObjectPtr);
};

class LocationChangeEvent : public eb::Event {
public:
  LocationChangeEvent(eb::ObjectPtr ptr) : eb::Event(ptr) {}
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
  ItemsFoundEvent(eb::ObjectPtr, Items);
  Items items;
};

class DigEvent : public eb::Event {
public:
  DigEvent(eb::ObjectPtr, std::shared_ptr<Cell>);
  std::shared_ptr<Cell> cell;
};

class CommitEvent : public eb::Event {
public:
  CommitEvent(eb::ObjectPtr s, int ap, bool q = false)
      : eb::Event(s), actionPoints(ap), silent(q) {}
  int actionPoints;
  bool silent;
};

typedef std::function<void()> PauseCallback;
class PauseEvent : public eb::Event {
public:
  PauseEvent(PauseCallback c) : eb::Event(nullptr), callback(c) {}
  PauseCallback callback;
};

typedef std::function<void(Direction)> DirectionCallback;
class DirectionEvent : public eb::Event {
public:
  DirectionEvent(DirectionCallback c) : eb::Event(nullptr), callback(c) {}
  DirectionCallback callback;
};

typedef std::function<void(std::shared_ptr<Cell>)> TargetCallback;
typedef std::function<bool(std::vector<std::shared_ptr<Cell>>)> CheckTargetCallback;
class TargetEvent : public eb::Event {
public:
  TargetEvent(TargetCallback c, CheckTargetCallback ct) : eb::Event(nullptr), callback(c), checkTarget(ct) {}
  TargetEvent(std::shared_ptr<Cell> st, TargetCallback c, CheckTargetCallback ct)
    : eb::Event(nullptr), startTarget(st), callback(c), checkTarget(ct) {}
  TargetCallback callback;
  CheckTargetCallback checkTarget;
  std::shared_ptr<Cell> startTarget;
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
  CommandEvent(eb::ObjectPtr);
};

class MoveCommandEvent : public CommandEvent {
public:
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
  DigCommandEvent(Direction);
  Direction direction;
};

class WalkCommandEvent : public CommandEvent {
public:
  WalkCommandEvent(Direction);
  Direction direction;
};

class AttackCommandEvent : public CommandEvent {
public:
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

class UseCommandEvent : public CommandEvent {
public:
  UseCommandEvent(std::shared_ptr<Item>);
  UseCommandEvent();
  std::shared_ptr<Item> item;
};

class WaitCommandEvent : public CommandEvent {
public:
  WaitCommandEvent();
};

class ZapCommandEvent : public CommandEvent {
public:
  ZapCommandEvent(eb::ObjectPtr, std::shared_ptr<Spell>);
  ZapCommandEvent();
  std::shared_ptr<Spell> spell;
};

class StairEvent : public CommandEvent {
public:
  StairEvent(StairType);
  StairEvent();
  StairType dir;
};
#endif // __EVENTS_H_
