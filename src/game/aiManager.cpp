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
  auto enemies = utils::castObjects<Enemy>(creatures);
  for (auto creature : enemies) {
    creature->actionPoints += ap;
  }

  bool done = false;
  while (!done) {
    currentLocation->updateLight(currentLocation->player);
    done = true;
    std::vector<std::thread*> handles;
    for (auto enemy : enemies) {
      if (enemy) {
        handles.push_back(new std::thread(
            [](std::shared_ptr<Enemy> enemy) {
              return enemy->prepareAiState();
            },
            enemy));
      }
    }
    auto n = 0;
    for (auto enemy : enemies) {
      auto t = handles.at(n);
      if (t->joinable()) {
        t->join();
      }
      n++;
      auto action = enemy->execAction();
      done = done ? action == std::nullopt : false;
    }
  }
  currentLocation->updateLight(currentLocation->player);
  log.stop(label);
  log.setThreshold(50);
}
