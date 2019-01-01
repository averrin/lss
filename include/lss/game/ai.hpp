#ifndef __AI_H_
#define __AI_H_
#include "lss/game/cell.hpp"
#include "lss/utils.hpp"

enum class AIType {
  NONE,
  AGGRESSIVE,
  PASSIVE,
};

struct AiState {
  bool exit = false;
  std::shared_ptr<Object> target;
  bool canSeeTarget;
  std::shared_ptr<Cell> targetCell;
  bool canSeeTargetCell = false;
  std::vector<std::shared_ptr<Cell>> path;
  bool nearTargetCell = false;
  bool nearTarget = false;
  std::vector<std::shared_ptr<Cell>> neighbors;
  bool canReachTarget = false;
  bool targetInTargetCell = false;
  bool inThrowRange = false;
  bool canThrow = false;
  std::vector<std::shared_ptr<Cell>> viewField;
};

class AiAction {
public:
  std::string name;
  int cost;
  AiAction(std::string n, int c) : name(n), cost(c){};
};

class WaitAction : public AiAction {
public:
  WaitAction(int c) : AiAction("WAIT", c) {}
};

class MoveAction : public AiAction {
public:
  MoveAction(int c) : AiAction(lu::blue("MOVE"), c) {}
};

class AttackAction : public AiAction {
public:
  AttackAction(int c) : AiAction(lu::bold(lu::red("ATTACK")), c) {}
};

class ThrowAction : public AiAction {
public:
  ThrowAction(int c) : AiAction(lu::red("THROW"), c) {}
};

class IdleAction : public AiAction {
public:
  IdleAction(int c) : AiAction(lu::gray("IDLE"), c) {}
};

#endif // __AI_H_
