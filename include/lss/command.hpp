#ifndef __COMMAND_H_
#define __COMMAND_H_

#include <optional>
#include <string>
#include <vector>

#include "Event.hpp"
#include "lss/game/events.hpp"

class Command {
public:
  Command(std::vector<std::string>);
  std::vector<std::string> aliases;
  virtual std::optional<std::shared_ptr<CommandEvent>>
      getEvent(std::string) = 0;
};

class MoveCommand : public Command {
public:
  MoveCommand();
  std::optional<std::shared_ptr<CommandEvent>> getEvent(std::string);
};

class QuitCommand : public Command {
public:
  QuitCommand();
  std::optional<std::shared_ptr<CommandEvent>> getEvent(std::string);
};

class PickCommand : public Command {
public:
  PickCommand();
  std::optional<std::shared_ptr<CommandEvent>> getEvent(std::string);
};

class DigCommand : public Command {
public:
  DigCommand();
  std::optional<std::shared_ptr<CommandEvent>> getEvent(std::string);
};

class WalkCommand : public Command {
public:
  WalkCommand();
  std::optional<std::shared_ptr<CommandEvent>> getEvent(std::string);
};

class AttackCommand : public Command {
public:
  AttackCommand();
  std::optional<std::shared_ptr<CommandEvent>> getEvent(std::string);
};

class EquipCommand : public Command {
public:
  EquipCommand();
  std::optional<std::shared_ptr<CommandEvent>> getEvent(std::string);
};

class HelpCommand : public Command {
public:
  HelpCommand();
  std::optional<std::shared_ptr<CommandEvent>> getEvent(std::string);
};

class InventoryCommand : public Command {
public:
  InventoryCommand();
  std::optional<std::shared_ptr<CommandEvent>> getEvent(std::string);
};

class DropCommand : public Command {
public:
  DropCommand();
  std::optional<std::shared_ptr<CommandEvent>> getEvent(std::string);
};

#endif // __COMMAND_H_
