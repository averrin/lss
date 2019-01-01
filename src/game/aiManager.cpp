#include "lss/game/aiManager.hpp"
#include "lss/game/ai.hpp"
#include "lss/game/enemy.hpp"
#include <algorithm>
#include <future>

AiManager::AiManager(std::shared_ptr<Location> l) : currentLocation(l) {}

void AiManager::processCommit(std::vector<std::shared_ptr<Creature>> creatures,
                              int ap) {
  if (ap == 0) {
    return;
  }

  log.setThreshold(100);
  auto label = "aiManager process";
  log.start(lu::cyan("AI"), label, true);
  for (auto creature : creatures) {
    creature->actionPoints += ap;
  }

  bool done = false;
  while (!done) {
    currentLocation->updateLight(currentLocation->player);
    done = true;
    std::vector<std::thread> handles;
    for (auto creature : creatures) {
      auto enemy = std::dynamic_pointer_cast<Enemy>(creature);
      if (enemy) {
        handles.push_back(std::thread(
            [](std::shared_ptr<Enemy> enemy) {
              return enemy->prepareAiState();
            },
            enemy));
      }
    }
    auto n = 0;
    for (auto creature : creatures) {
      if (handles[n].joinable()) {
        handles[n].join();
      }
      n++;
      auto enemy = std::dynamic_pointer_cast<Enemy>(creature);
      auto action = enemy->execAction();
      done = done ? action == std::nullopt : false;
    }
  }
  currentLocation->updateLight(currentLocation->player);
  log.stop(label);
  log.setThreshold(50);
}
