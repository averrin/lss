#include <memory>

#include "lss/LSSApp.hpp"
#include "lss/actions.hpp"
#include "lss/commands.hpp"
#include "lss/generator/room.hpp"
#include "lss/modes.hpp"
#include "lss/utils.hpp"

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

// TODO: to utils
std::optional<std::string> getDir(int code) {
  switch (code) {
  case SDL_SCANCODE_J:
    return "s"s;
  case SDL_SCANCODE_H:
    return "w"s;
  case SDL_SCANCODE_L:
    return "e"s;
  case SDL_SCANCODE_K:
    return "n"s;
  case SDL_SCANCODE_Y:
    return "nw"s;
  case SDL_SCANCODE_U:
    return "ne"s;
  case SDL_SCANCODE_B:
    return "sw"s;
  case SDL_SCANCODE_N:
    return "se"s;
  }
  return std::nullopt;
}

ModeManager::ModeManager(){};

void ModeManager::processKey(KeyEvent event) {
  state_machine.process_event(KeyPressedEvent(event));
}

void ModeManager::processEvent(std::shared_ptr<LssEvent> event) {
  state_machine.process_event(*event);
}

void ModeManager::toNormal() { state_machine.process_event(ModeExitedEvent{}); }

void ModeManager::toDirection() {
  state_machine.process_event(EnableModeEvent{Modes::DIRECTION});
}

void ModeManager::toHelp() {
  state_machine.process_event(EnableModeEvent{Modes::HELP});
}

void ModeManager::toInventory() {
  state_machine.process_event(EnableModeEvent{Modes::INVENTORY});
}

void ModeManager::toObjectSelect() {
  state_machine.process_event(EnableModeEvent{Modes::OBJECTSELECT});
}

void ModeManager::toGameOver() {
  state_machine.process_event(EnableModeEvent{Modes::GAMEOVER});
}

void ModeManager::toInspect() {
  state_machine.process_event(EnableModeEvent{Modes::INSPECT});
}

void ModeManager::toPause() {
  state_machine.process_event(EnableModeEvent{Modes::PAUSE});
}

void ModeManager::toTarget() {
  state_machine.process_event(EnableModeEvent{Modes::TARGET});
}

void HintsMode::processEvent(std::shared_ptr<LssEvent> event) {}

Mode::Mode(LSSApp *a) : app(a){};

bool HintsMode::processKey(KeyEvent event) { return false; }

bool TargetMode::processKey(KeyEvent event) {
  switch (event.getCode()) {
  case SDL_SCANCODE_J:
  case SDL_SCANCODE_H:
  case SDL_SCANCODE_L:
  case SDL_SCANCODE_K:
  case SDL_SCANCODE_Y:
  case SDL_SCANCODE_U:
  case SDL_SCANCODE_B:
  case SDL_SCANCODE_N: {
    auto d = getDir(event.getCode());
    if (d == std::nullopt)
      break;
    auto nc = app->hero->currentLocation->getCell(
        app->hero->currentLocation
            ->cells[app->state->cursor.y][app->state->cursor.x],
        *utils::getDirectionByName(*d));
    auto location = app->hero->currentLocation;
    auto line = location->getLine(app->hero->currentCell, nc);
    if (!checkTarget(line))
      break;

    app->state->selection.clear();
    app->state->cursor = {nc->x, nc->y};
    auto cell = location->cells[app->state->cursor.y][app->state->cursor.x];
    for (auto c : line) {
      app->state->selection.push_back({{c->x, c->y}, "#6c6d79"});
    }
    app->state->invalidate();
    // render();
    return true;
  } break;
  case SDL_SCANCODE_T:
  case SDL_SCANCODE_RETURN:
    callback(app->hero->currentLocation
                 ->cells[app->state->cursor.y][app->state->cursor.x]);
    return true;
    break;
  }
  return false;
}

bool ObjectSelectMode::processKey(KeyEvent event) {
  std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  auto ch = event.getChar();
  if (event.isShiftDown()) {
    ch = toupper(ch);
  }
  auto index = letters.find(ch);
  if (objects.size() > index) {
    if (callback(objects[index])) {
      return true;
    }
  }
  return false;
}

bool PauseMode::processKey(KeyEvent event) {
  if (callback != nullptr) {
    callback();
  }
  app->modeManager.toNormal();
  app->invalidate("unpause");
  return true;
}

bool GameOverMode::processKey(KeyEvent event) {
  switch (event.getCode()) {
  case SDL_SCANCODE_Q:
    app->processCommand("quit");
    return true;
    break;
  case SDL_SCANCODE_R:
    app->modeManager.toNormal();
    app->hero->currentLocation->leave(app->hero);

    app->bgRunning = false;
    app->bgThread.join();
    app->startGame();
    return true;
    break;
  }
  return false;
}

bool InspectMode::processKey(KeyEvent event) {
  switch (event.getCode()) {
  case SDL_SCANCODE_J:
  case SDL_SCANCODE_H:
  case SDL_SCANCODE_L:
  case SDL_SCANCODE_K:
  case SDL_SCANCODE_Y:
  case SDL_SCANCODE_U:
  case SDL_SCANCODE_B:
  case SDL_SCANCODE_N: {
    auto d = getDir(event.getCode());
    if (d == std::nullopt)
      break;
    auto nc = app->hero->currentLocation->getCell(
        app->hero->currentLocation
            ->cells[app->state->cursor.y][app->state->cursor.x],
        *utils::getDirectionByName(*d));
    app->state->cursor = {nc->x, nc->y};
    app->state->invalidate();
    render();
    return true;
  } break;
  }
  return false;
}

void InspectMode::render() {
  auto location = app->hero->currentLocation;
  auto cell = location->cells[app->state->cursor.y][app->state->cursor.x];
  auto objects = location->getObjects(cell);
  auto cc = app->hero->currentCell;
  auto check = "<span color='green'>✔</span>";
  app->state->selection.clear();

  auto line = location->getLine(app->hero->currentCell, cell);
  for (auto c : line) {
    app->state->selection.push_back({{c->x, c->y}, "#11f"});
  }

  app->inspectState->setContent(
      {F(fmt::format("Selected cell: <b>{}.{}</b>", cell->x, cell->y))});
  app->inspectState->appendContent(State::END_LINE);

  if (!app->debug && !app->hero->canSee(cell)) {
    app->inspectState->appendContent(
        {F(fmt::format("You cannot see this cell"))});
    app->inspectState->appendContent(State::END_LINE);
    return;
  }

  app->inspectState->appendContent(
      {F(fmt::format("Type: <b>{}</b>", cell->type.name))});
  app->inspectState->appendContent(State::END_LINE);
  if (cell->type == CellType::UNKNOWN)
    return;
  app->inspectState->appendContent(
      {F(fmt::format("Type <b>PASS</b>THROUGH: [<b>{}</b>]",
                     cell->type.passThrough ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent(
      {F(fmt::format("Type <b>SEE</b>THROUGH: [<b>{}</b>]",
                     cell->type.passThrough ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(fmt::format(
      "<b>PASS</b>THROUGH: [<b>{}</b>]", cell->passThrough ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(fmt::format(
      "<b>SEE</b>THROUGH: [<b>{}</b>]", cell->passThrough ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(fmt::format(
      "Illuminated: [<b>{}</b>]", cell->illuminated ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent(
      {F(fmt::format("Illumination: <b>{}</b>", cell->illumination))});
  app->inspectState->appendContent(State::END_LINE);
  auto f = app->state->fragments[cell->y * app->state->width + cell->x];
  // app->inspectState->appendContent({F(fmt::format(
  //     "Cell Illumination: <b>{}</b>", f->alpha))});
  // app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(
      fmt::format("Cell features count: <b>{}</b>", cell->features.size()))});
  app->inspectState->appendContent(State::END_LINE);
  if (cell->features.size() > 0) {
    app->inspectState->appendContent({F("<b>Cell Features</b>")});
    app->inspectState->appendContent(State::END_LINE);
  }
  for (auto f : cell->features) {
    if (f == CellFeature::BLOOD) {
      app->inspectState->appendContent(
          {F(fmt::format("BLOOD: [<b>{}</b>]", check))});
    }
    if (f == CellFeature::CAVE) {
      app->inspectState->appendContent(
          {F(fmt::format("CAVE: [<b>{}</b>]", check))});
    }
    app->inspectState->appendContent(State::END_LINE);
  }
  app->inspectState->appendContent(State::END_LINE);
  if (cell->room == nullptr) {
    app->inspectState->appendContent(
        {F(fmt::format("<b>Room is nullptr!</b>"))});
    app->inspectState->appendContent(State::END_LINE);
  } else {
    app->inspectState->appendContent(
        {F(fmt::format("Room @ <b>{}.{} [{}x{}]</b>", cell->room->x,
                       cell->room->y, cell->room->width, cell->room->height))});
    app->inspectState->appendContent(State::END_LINE);
    app->inspectState->appendContent(
        {F(fmt::format("HALL: [<b>{}</b>]",
                       cell->room->type == RoomType::HALL ? check : " "))});
    app->inspectState->appendContent(State::END_LINE);
    app->inspectState->appendContent(
        {F(fmt::format("PASSAGE: [<b>{}</b>]",
                       cell->room->type == RoomType::PASSAGE ? check : " "))});
    app->inspectState->appendContent(State::END_LINE);
    app->inspectState->appendContent({F(fmt::format(
        "Room features count: <b>{}</b>", cell->room->features.size()))});
    app->inspectState->appendContent(State::END_LINE);
    if (app->debug) {
      for (auto c : cell->room->cells) {
        app->state->selection.push_back({{c->x, c->y}, "#f11"});
      }
    }

    if (cell->room->features.size() > 0) {
      app->inspectState->appendContent({F("<b>Room Features</b>")});
      app->inspectState->appendContent(State::END_LINE);
    }
    for (auto f : cell->room->features) {
      if (f == RoomFeature::DUNGEON) {
        app->inspectState->appendContent(
            {F(fmt::format("DUNGEON: [<b>{}</b>]", check))});
      }
      if (f == RoomFeature::CAVE) {
        app->inspectState->appendContent(
            {F(fmt::format("CAVE: [<b>{}</b>]", check))});
      }
      app->inspectState->appendContent(State::END_LINE);
    }
    app->inspectState->appendContent(State::END_LINE);
  }

  app->inspectState->appendContent(
      {F(fmt::format("Light sources: <b>{}</b>", cell->lightSources.size()))});

  for (auto ls : cell->lightSources) {
    auto c = ls->currentCell;
    app->state->selection.push_back({{c->x, c->y}, "#1f1"});
  }
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(fmt::format(
      "Hero: [<b>{}</b>]", cell == app->hero->currentCell ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent(
      {F(fmt::format("Hero can <b>pass</b>: [<b>{}</b>]",
                     cell->canPass(app->hero->traits) ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);

  app->inspectState->appendContent(
      {F(fmt::format("Hero can <b>see</b>: [<b>{}</b>]",
                     app->hero->canSee(cell) ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(
      fmt::format("Distance to hero: <b>{}</b>",
                  sqrt(pow(cc->x - cell->x, 2) + pow(cc->y - cell->y, 2))))});
  app->inspectState->appendContent(State::END_LINE);

  if (cell->type == CellType::WALL)
    return;

  auto allEnemies = utils::castObjects<Enemy>(location->objects);
  for (auto e : allEnemies) {
    if (e->canSee(cell)) {
      app->inspectState->appendContent(
          {F(fmt::format("<b>{} @ {}.{}</b> can see: [<b>{}</b>]", e->type.name,
                         e->currentCell->x, e->currentCell->y,
                         e->canSee(cell) ? check : " "))});
      app->state->selection.push_back(
          {{e->currentCell->x, e->currentCell->y}, "#fff"});
      app->inspectState->appendContent(State::END_LINE);
    }
  }

  app->inspectState->appendContent(
      {F(fmt::format("Objects count: <b>{}</b>", objects.size()))});
  app->inspectState->appendContent(State::END_LINE);

  if (objects.size() > 0) {
    auto isDoor = utils::castObjects<Door>(objects).size() > 0;
    app->inspectState->appendContent(
        {F(fmt::format("Door: [<b>{}</b>]", isDoor ? check : " "))});
    app->inspectState->appendContent(State::END_LINE);
    // auto isTorch = utils::castObjects<TorchStand>(objects).size() > 0;
    // app->inspectState->appendContent(
    //     {F(fmt::format("Torch: [<b>{}</b>]", isTorch ? check : " "))});
    // app->inspectState->appendContent(State::END_LINE);
    // auto isStatue = utils::castObjects<Statue>(objects).size() > 0;
    // app->inspectState->appendContent(
    //     {F(fmt::format("Statue: [<b>{}</b>]", isStatue ? check : " "))});
    // app->inspectState->appendContent(State::END_LINE);
    auto enemies = utils::castObjects<Enemy>(objects);
    if (enemies.size() > 0) {
      std::vector<std::string> enemyNames;
      for (auto e : enemies) {
        enemyNames.push_back(e->type.name);
        app->inspectState->appendContent(
            {F(fmt::format("<b>{} @ {}.{}</b> can see HERO: [<b>{}</b>]",
                           e->type.name, e->currentCell->x, e->currentCell->y,
                           e->canSee(cc) ? check : " "))});
        app->inspectState->appendContent(State::END_LINE);
      }
      app->inspectState->appendContent({F(
          fmt::format("Enemies: <b>{}</b>", utils::join(enemyNames, ", ")))});
      app->inspectState->appendContent(State::END_LINE);
    }
    auto items = utils::castObjects<Item>(objects);
    if (items.size() > 0) {
      std::vector<std::string> itemNames;
      for (auto i : items) {
        itemNames.push_back(i->getFullTitle());
      }
      app->inspectState->appendContent(
          {F(fmt::format("Items: <b>{}</b>", utils::join(itemNames, ", ")))});
      app->inspectState->appendContent(State::END_LINE);
    }
  }
}

bool NormalMode::processKey(KeyEvent event) {
  switch (event.getCode()) {
    // TODO: create command
  case SDL_SCANCODE_T: {

    app->objectSelectMode->setHeader(F("Items to throw: "));

    Items dropable(app->hero->inventory.size());
    auto it =
        std::copy_if(app->hero->inventory.begin(), app->hero->inventory.end(),
                     dropable.begin(), [](std::shared_ptr<Item> item) {
                       return !item->equipped;
                     });

    dropable.resize(std::distance(dropable.begin(), it));

    app->objectSelectMode->setObjects(utils::castObjects<Object>(dropable));

    Formatter formatter = [](std::shared_ptr<Object> o, std::string letter) {
      if (auto item = std::dynamic_pointer_cast<Item>(o)) {
        return fmt::format("<span weight='bold'>{}</span> - {}", letter,
                           item->getFullTitle());
      }
      return "Unknown error"s;
    };
    app->objectSelectMode->setFormatter(formatter);

    app->objectSelectMode->setCallback([=](std::shared_ptr<Object> o) {
      auto item = std::dynamic_pointer_cast<Item>(o);
      app->state->selection.clear();
      app->targetMode->setCallback([=](auto cell) {
        app->hero->throwItem(item, cell);
        app->modeManager.toNormal();
        app->statusLine->setContent(State::normal_mode);
        return true;
      });
      app->targetMode->setCheckTarget([&](auto line) {
        auto cell = line.back();
        if (!cell->type.passThrough)
          return false;
        if (line.size() > 5)
          return false;
        auto pti = std::find_if(line.begin(), line.end(), [&](auto c) {
          return !c->passThrough && c != line.back() &&
                 c != app->hero->currentCell;
        });
        if (pti != line.end())
          return false;
        return true;
      });
      app->modeManager.toTarget();
      app->statusLine->setContent(State::target_mode);
      auto n = app->hero->currentLocation->getNeighbors(app->hero->currentCell);
      auto s = std::find_if(n.begin(), n.end(),
                            [](auto c) { return c->type.passThrough; });
      app->state->cursor = {(*s)->x, (*s)->y};
      app->state->setSelect(true);
      app->state->invalidate();
      return true;
    });

    app->objectSelectMode->render(app->objectSelectState);
    app->modeManager.toObjectSelect();

  } break;
  case SDL_SCANCODE_F1:
    app->debug = !app->debug;
    app->hero->commit("toggle debug", 0);
    break;
  case SDL_SCANCODE_J:
  case SDL_SCANCODE_H:
  case SDL_SCANCODE_L:
    // TODO: create command
    if (event.isShiftDown()) {
      auto slot = app->hero->getSlot(WearableType::LIGHT);
      if (slot->item != nullptr) {
        app->hero->unequip(slot);
        app->hero->emitsLight = false;
      } else {
        if (auto torch = std::find_if(
                app->hero->inventory.begin(), app->hero->inventory.end(),
                [](auto i) {
                  return i->type.wearableType == WearableType::LIGHT &&
                         i->durability > 0;
                });
            torch != app->hero->inventory.end()) {
          app->hero->equip(slot, *torch);
          app->hero->emitsLight = true;
        }
      }
      app->hero->currentLocation->invalidate();
      app->hero->commit("equip light", 0);
      app->hero->calcViewField();
      app->hero->currentLocation->updateView(app->hero);
      app->hero->commit("equip light", 0);
      app->invalidate();
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
      auto d = getDir(event.getCode());
      if (d == std::nullopt)
        break;
      app->processCommand(*d);
    } else {
      auto ds =
          std::vector<std::string>{"e", "s", "w", "n", "nw", "ne", "sw", "se"};
      auto d = ds[rand() % ds.size()];
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
    app->hero->currentCell = app->hero->currentLocation->exitCell;
    app->processCommand("down");
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
          enemy->currentCell = app->hero->currentCell;
          enemy->currentLocation = app->hero->currentLocation;

          enemy->handlers.push_back(
              eb::EventBus::AddHandler<CommitEvent>(*enemy, app->hero));
          enemy->calcViewField();
          app->hero->currentLocation->objects.push_back(enemy);
          app->modeManager.toNormal();
          return true;
        });

        app->objectSelectMode->render(app->objectSelectState);
        app->modeManager.toObjectSelect();
      } else {
        app->objectSelectMode->setHeader(F("Items to spawn: "));
        app->objectSelectMode->setObjects(
            utils::castObjects<Object>(Prototype::ALL));

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
          item->currentCell = app->hero->currentCell;
          app->hero->currentLocation->objects.push_back(item);
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

bool DirectionMode::processKey(KeyEvent event) {
  std::optional<std::string> dirName = getDir(event.getCode());
  if (dirName != std::nullopt) {
    app->modeManager.toNormal();
    app->statusLine->setContent(State::normal_mode);
    callback(*utils::getDirectionByName(*dirName));
    return true;
  }
  return false;
}

bool InsertMode::processKey(KeyEvent event) {
  if (event.getCode() != SDL_SCANCODE_SLASH &&
      event.getCode() != SDL_SCANCODE_RETURN &&
      event.getCode() != SDL_SCANCODE_BACKSPACE) {
    app->typedCommand += event.getChar();
  } else if (event.getCode() == SDL_SCANCODE_BACKSPACE) {
    if (app->typedCommand.length() > 0) {
      app->typedCommand.erase(app->typedCommand.length() - 1,
                              app->typedCommand.length());
    } else {
      app->modeManager.toNormal();
      app->statusLine->setContent(State::normal_mode);
      app->typedCommand = "";
      return true;
    }
  } else if (event.getCode() == SDL_SCANCODE_RETURN) {
    app->modeManager.toNormal();
    app->statusLine->setContent(State::normal_mode);
    app->processCommand(app->typedCommand);
    app->typedCommand = "";
    return true;
  }
  app->statusLine->setContent(State::insert_mode);
  if (app->typedCommand.length() != 0) {
    app->statusLine->setContent(
        {State::insert_mode.front(), F(app->typedCommand)});
  }
  return false;
}

void HelpMode::render(std::shared_ptr<State> state) {
  state->setContent({header});
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);
  state->appendContent(State::HELP);
}

void GameOverMode::render(std::shared_ptr<State> state) {
  auto hero = app->hero;
  state->setContent({});
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);
  state->appendContent(State::GAMEOVER);
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);
  int gold = 0;

  for (auto i : hero->inventory) {
    if (i->type == ItemType::GOLD_COINS) {
      gold = i->count;
    }
  }

  state->appendContent(
      F(fmt::format("<b>{}</b> [{}]", hero->name, hero->level)));
  state->appendContent(State::END_LINE);
  state->appendContent(F(fmt::format("EXP:             <b>{}</b>", hero->exp)));
  state->appendContent(State::END_LINE);
  state->appendContent(F(fmt::format("GOLD:            <b>{}</b>", gold)));
  state->appendContent(State::END_LINE);
  state->appendContent(
      F(fmt::format("DMG TAKEN:       <b>{}</b>", hero->report.damageTaken)));
  state->appendContent(State::END_LINE);
  state->appendContent(F(
      fmt::format("DMG INFLICTED:   <b>{}</b>", hero->report.damageInflicted)));
  state->appendContent(State::END_LINE);
  state->appendContent(F(
      fmt::format("DMG DEFLECTED:   <b>{}</b>", hero->report.damageDeflected)));
  state->appendContent(State::END_LINE);
  state->appendContent(
      F(fmt::format("AP COMMITED:     <b>{}</b>", hero->report.apCommited)));
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);

  state->appendContent(F(fmt::format("<b>KILLS</b>:")));
  state->appendContent(State::END_LINE);
  for (auto [name, kills] : hero->report.kills) {
    state->appendContent(
        F(fmt::format("{:14}   <b>{}</b>", name + ":", kills)));
    state->appendContent(State::END_LINE);
  }

  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);
  state->appendContent(
      F("Press [<span color='{{orange}}' weight='bold'>q</span>] for exit"
        " or [<span color='{{orange}}' weight='bold'>r</span>] for restart"));
}

void InventoryMode::render(std::shared_ptr<State> state) {
  state->setContent({header});
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);
  if (objects.size() == 0) {
    state->appendContent(F("<span color='grey'>Nothing here.</span>"));
    return;
  }

  namespace ic = ItemCategories;
  std::vector<ItemCategory> categories = {ic::ARMOR,     ic::WEAPONS,
                                          ic::JEWELLERY, ic::CONSUMABLES,
                                          ic::LIGHT,     ic::MISC};
  auto items = utils::castObjects<Item>(objects);
  for (auto cat : categories) {
    Fragments fragments;
    for (auto item : items) {
      if (item->type.category != cat)
        continue;
      fragments.push_back(F(fmt::format(
          "     {} {} {}",
          item->equipped ? "<span weight='bold' color='#F7CA88'>•</span>" : "•",
          fmt::format(!item->identified && item->type.name != item->name
                          ? "<i>{}</i>"
                          : "{}",
                      item->getFullTitle()),
          item->equipped ? "<span color='gray'>&lt;equipped&gt;</span>" : "")));
      fragments.push_back(State::END_LINE.front());
    }
    if (fragments.size() == 0)
      continue;
    state->appendContent(F(fmt::format("<b>{}</b>", cat.name)));
    state->appendContent(State::END_LINE);
    state->appendContent(fragments);
    state->appendContent(State::END_LINE);
  }
}

// TODO: scroll
void ObjectSelectMode::render(std::shared_ptr<State> state) {
  state->setContent({header});
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);

  if (objects.size() == 0) {
    state->appendContent(F("<span color='grey'>Nothing to choose.</span>"));
    return;
  }

  std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  auto n = 0;
  for (auto o : objects) {
    state->appendContent(F(formatter(o, std::string{letters[n]})));
    state->appendContent(State::END_LINE);
    n++;
  }
}
