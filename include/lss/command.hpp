#ifndef __COMMAND_H_
#define __COMMAND_H_

#include <optional>
#include <string>
#include <vector>

#include "Event.hpp"
#include "lss/game/events.hpp"
using namespace std::string_literals;

template <class T>
class Command {
public:
  Command(std::vector<std::string> a): aliases(a) {}
  std::vector<std::string> aliases;
  virtual std::optional<std::shared_ptr<T>>
  getEvent(std::string) { return std::make_shared<T>();};
};

class MoveCommand : public Command<MoveCommandEvent> {
public:
MoveCommand()
    : Command({"move", "m"s, "n"s, "e"s, "s"s, "w"s, "nw", "ne", "se", "sw"}){}
  std::optional<std::shared_ptr<MoveCommandEvent>> getEvent(std::string);
};
class DigCommand : public Command<DigCommandEvent> {
public:
DigCommand() : Command({"dig", "d"s}) {}
  std::optional<std::shared_ptr<DigCommandEvent>> getEvent(std::string);
};
class WalkCommand : public Command<WalkCommandEvent> {
public:
WalkCommand() : Command({"walk"}) {}
  std::optional<std::shared_ptr<WalkCommandEvent>> getEvent(std::string);
};
class AttackCommand : public Command<AttackCommandEvent> {
public:
AttackCommand() : Command({"attack", "a"s}) {}
  std::optional<std::shared_ptr<AttackCommandEvent>> getEvent(std::string);
};
class QuitCommand:     public Command<QuitCommandEvent> {
public:
QuitCommand() : Command({"quit", "q"s}) {}
};
class PickCommand:     public Command<PickCommandEvent> {
public:
PickCommand() : Command({"pick", "p"s}) {}
};
class EquipCommand:    public Command<EquipCommandEvent> {
public:
EquipCommand() : Command({"equip", "eq"s}) {}
};
class HelpCommand:     public Command<HelpCommandEvent> {
public:
HelpCommand() : Command({"help", "h"s}) {}
};
class InventoryCommand: public Command<InventoryCommandEvent> {
public:
InventoryCommand() : Command({"inventory", "i"s}) {}
};
class DropCommand:     public Command<DropCommandEvent> {
public:
DropCommand() : Command({"drop", "dr"s}) {}
};
class WaitCommand:     public Command<WaitCommandEvent> {
public:
WaitCommand() : Command({"wait"}) {}
};
class ZapCommand:      public Command<ZapCommandEvent> {
public:
ZapCommand() : Command({"zap", "z"s}) {}
};



#endif // __COMMAND_H_
