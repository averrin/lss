#include "lss/ui/normalMode.hpp"
#include "lss/LSSApp.hpp"
#include "lss/fragment.hpp"
#include "lss/game/content/enemies.hpp"
#include "lss/utils.hpp"

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

bool NormalMode::processKey(KeyEvent event) {
  switch (event.getCode()) {
  case SDL_SCANCODE_2:
    if (event.isShiftDown()) {
      app->processCommand("hero");
    }
    break;
  case SDL_SCANCODE_T:
    app->processCommand("throw");
    break;
  case SDL_SCANCODE_F1:
    app->debug = !app->debug;
    app->invalidate();
    break;
  case SDL_SCANCODE_J:
  case SDL_SCANCODE_H:
  case SDL_SCANCODE_L:
    if (event.isShiftDown()) {
      app->processCommand("light");
      break;
    }
  case SDL_SCANCODE_K:
  case SDL_SCANCODE_Y:
  case SDL_SCANCODE_U:
    if (event.isShiftDown()) {
      app->processCommand("use");
      break;
    }
  case SDL_SCANCODE_B:
  case SDL_SCANCODE_N: {
    if (!app->hero->hasTrait(Traits::CONFUSED)) {
      auto d = utils::getDir(event.getCode());
      if (!d)
        break;
      app->processCommand(*d);
    } else {
      // TODO: move to utils
      auto ds =
          std::vector<std::string>{"e", "s", "w", "n", "nw", "ne", "sw", "se"};
      auto d = ds.at(rand() % ds.size());
      app->processCommand(d);
    }
  } break;
  case SDL_SCANCODE_PERIOD:
    if (event.isShiftDown()) {
      app->processCommand("down");
    } else {
      app->processCommand("wait");
      app->invalidate();
    }
    break;
  case SDL_SCANCODE_Q:
    app->processCommand("quit");
    break;
  case SDL_SCANCODE_P:
    app->processCommand("pick");
    break;
  case SDL_SCANCODE_COMMA:
    if (event.isShiftDown()) {
      app->processCommand("up");
    } else {
      app->processCommand("pick");
    }
    break;
  case SDL_SCANCODE_D:
    if (!event.isShiftDown()) {
      app->directionMode->setCallback([](auto dir) {
        auto e = std::make_shared<DigCommandEvent>(dir);
        eb::EventBus::FireEvent(*e);
      });
      app->modeManager.toDirection();
      app->statusLine->setContent({F("Dig: "), State::direction_mode.front()});
    } else {
      app->processCommand("drop");
    }
    break;
  case SDL_SCANCODE_A:
    app->directionMode->setCallback([](auto dir) {
      auto e = std::make_shared<AttackCommandEvent>(dir);
      eb::EventBus::FireEvent(*e);
    });
    app->modeManager.toDirection();
    app->statusLine->setContent({F("Attack: "), State::direction_mode.front()});
    break;
  case SDL_SCANCODE_W:
    app->directionMode->setCallback([](auto dir) {
      auto e = std::make_shared<WalkCommandEvent>(dir);
      eb::EventBus::FireEvent(*e);
    });
    app->modeManager.toDirection();
    app->statusLine->setContent({F("Walk: "), State::direction_mode.front()});
    break;
  case SDL_SCANCODE_E:
    app->processCommand("equip");
    break;
    break;
  case SDL_SCANCODE_I:
    if (event.isShiftDown()) {
      app->modeManager.toInspect();
      app->statusLine->setContent(State::inspect_mode);
      app->state->cursor = {app->hero->currentCell->x,
                            app->hero->currentCell->y};
      app->state->setSelect(true);
      app->state->invalidate();
      app->inspectMode->render();
      app->hero->commit("inspect", 0);
    } else {
      app->processCommand("inventory");
    }
    break;
  case SDL_SCANCODE_Z:
    app->processCommand("zap");
    break;
  case SDL_SCANCODE_R:
    if (!event.isShiftDown()) {
      app->hero->setCurrentCell(app->hero->currentLocation->exitCell);
      app->processCommand("down");
    } else {
      app->objectSelectMode->setHeader(F("Lootbox for open: "));

      std::vector<std::shared_ptr<LootBoxHolder>> boxes;
      boxes.resize(LootBoxes::ALL.size());
      std::transform(LootBoxes::ALL.begin(), LootBoxes::ALL.end(),
                     boxes.begin(),
                     [](auto b) { return std::make_shared<LootBoxHolder>(b); });
      app->objectSelectMode->setObjects(
          utils::castObjects<Object>(boxes, true));

      Formatter formatter = [](std::shared_ptr<Object> o, std::string letter) {
        if (auto h = std::dynamic_pointer_cast<LootBoxHolder>(o)) {
          return fmt::format(
              "<span weight='bold'>{}</span> - {} items, {} children, {}",
              letter, h->box.items.size(), h->box.children.size(),
              h->box.exclusive);
        }
        return "Unknown error"s;
      };
      app->objectSelectMode->setFormatter(formatter);

      app->objectSelectMode->setCallback([&](std::shared_ptr<Object> o) {
        auto box = std::dynamic_pointer_cast<LootBoxHolder>(o)->box;
        for (auto item : box.open()) {
          item->setCurrentCell(app->hero->currentCell);
          app->hero->currentLocation->addObject<Item>(item);
        }
        app->modeManager.toNormal();
        return true;
      });

      app->objectSelectMode->render(app->objectSelectState);
      app->modeManager.toObjectSelect();
    }
    break;
  case SDL_SCANCODE_SLASH:
    if (event.isShiftDown()) {
      app->processCommand("help");
    }
    break;
  case SDL_SCANCODE_S:
    if (app->debug) {
      if (event.isShiftDown()) {
        app->objectSelectMode->setHeader(F("Enemies to spawn: "));
        std::vector<std::shared_ptr<EnemySpecHolder>> holders;
        for (auto s : EnemyType::ALL) {
          holders.push_back(std::make_shared<EnemySpecHolder>(s));
        }
        app->objectSelectMode->setObjects(utils::castObjects<Object>(holders));

        Formatter formatter = [](std::shared_ptr<Object> o,
                                 std::string letter) {
          if (auto sh = std::dynamic_pointer_cast<EnemySpecHolder>(o)) {
            return fmt::format("<span weight='bold'>{}</span> - {}", letter,
                               sh->spec.name);
          }
          return "Unknown error"s;
        };
        app->objectSelectMode->setFormatter(formatter);

        app->objectSelectMode->setCallback([&](std::shared_ptr<Object> o) {
          auto sh = std::dynamic_pointer_cast<EnemySpecHolder>(o);
          auto enemy = std::make_shared<Enemy>(sh->spec);
          enemy->setCurrentCell(app->hero->currentCell);
          enemy->currentLocation = app->hero->currentLocation;

          enemy->handlers.push_back(
              eb::EventBus::AddHandler<CommitEvent>(*enemy, app->hero));
          enemy->calcViewField();
          app->hero->currentLocation->addObject<Enemy>(enemy);
          app->modeManager.toNormal();
          return true;
        });

        app->objectSelectMode->render(app->objectSelectState);
        app->modeManager.toObjectSelect();
      } else {
        app->objectSelectMode->setHeader(F("Items to spawn: "));

        std::vector<std::shared_ptr<Item>> allItems;
        for (auto v : Prototype::ALL) {
          allItems.insert(allItems.end(), v.begin(), v.end());
        }
        app->objectSelectMode->setObjects(
            utils::castObjects<Object>(allItems, true));

        Formatter formatter = [](std::shared_ptr<Object> o,
                                 std::string letter) {
          if (auto item = std::dynamic_pointer_cast<Item>(o)) {
            return fmt::format("<span weight='bold'>{}</span> - {}", letter,
                               item->getTitle(true));
          }
          return "Unknown error"s;
        };
        app->objectSelectMode->setFormatter(formatter);

        app->objectSelectMode->setCallback([&](std::shared_ptr<Object> o) {
          auto item = std::dynamic_pointer_cast<Item>(o)->roll();
          item->setCurrentCell(app->hero->currentCell);
          app->hero->currentLocation->addObject<Item>(item);
          app->modeManager.toNormal();
          return true;
        });

        app->objectSelectMode->render(app->objectSelectState);
        app->modeManager.toObjectSelect();
      }
    }
    break;
  default:
    return false;
    break;
  }
  return true;
}
