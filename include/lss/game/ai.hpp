#ifndef __AI_H_
#define __AI_H_
#include "lss/game/cell.hpp"

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
  ;
  std::vector<std::shared_ptr<Cell>> path;
  bool nearTargetCell = false;
  bool nearTarget = false;
  std::vector<std::shared_ptr<Cell>> neighbors;
  bool canReachTarget = false;
  bool targetInTargetCell = false;
  bool inThrowRange = false;
  bool canThrow = false;
};

#endif // __AI_H_
