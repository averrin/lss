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
  case KeyEvent::KEY_j:
    return "s";
  case KeyEvent::KEY_h:
    return "w";
  case KeyEvent::KEY_l:
    return "e";
  case KeyEvent::KEY_k:
    return "n";
  case KeyEvent::KEY_y:
    return "nw";
  case KeyEvent::KEY_u:
    return "ne";
  case KeyEvent::KEY_b:
    return "sw";
  case KeyEvent::KEY_n:
    return "se";
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

void HintsMode::processEvent(std::shared_ptr<LssEvent> event) {}

Mode::Mode(LSSApp *a) : app(a){};

bool HintsMode::processKey(KeyEvent event) { return false; }

bool ObjectSelectMode::processKey(KeyEvent event) {
  std::string letters = "abcdefghijklmnopqrstuvwxyz";
  auto index = letters.find(event.getChar());
  if (objects.size() > index) {
    if (callback(objects[index])) {
      return true;
    }
  }
  return false;
}

bool GameOverMode::processKey(KeyEvent event) {
  switch (event.getCode()) {
  case KeyEvent::KEY_q:
    app->processCommand("quit");
    return true;
    break;
  }
  return false;
}

bool InspectMode::processKey(KeyEvent event) {
  switch (event.getCode()) {
  case KeyEvent::KEY_j:
  case KeyEvent::KEY_h:
  case KeyEvent::KEY_l:
  case KeyEvent::KEY_k:
  case KeyEvent::KEY_y:
  case KeyEvent::KEY_u:
  case KeyEvent::KEY_b:
  case KeyEvent::KEY_n: {
    auto d = getDir(event.getCode());
    if (d == std::nullopt)
      break;
    auto nc = app->hero->getCell(
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
        app->state->selection.push_back({{c->x, c->y}, "#111"});
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
    auto isTorch = utils::castObjects<TorchStand>(objects).size() > 0;
    app->inspectState->appendContent(
        {F(fmt::format("Torch: [<b>{}</b>]", isTorch ? check : " "))});
    app->inspectState->appendContent(State::END_LINE);
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
  case KeyEvent::KEY_F1:
    app->debug = !app->debug;
    app->hero->commit("toggle debug", 0);
    break;
  case KeyEvent::KEY_j:
  case KeyEvent::KEY_h:
  case KeyEvent::KEY_l:
  case KeyEvent::KEY_k:
  case KeyEvent::KEY_y:
  case KeyEvent::KEY_u:
  case KeyEvent::KEY_b:
  case KeyEvent::KEY_n: {
    auto d = getDir(event.getCode());
    if (d == std::nullopt)
      break;
    app->processCommand(*d);
  } break;
  case KeyEvent::KEY_PERIOD:
    if (event.isShiftDown()) {
      app->processCommand("down");
    } else {
      app->processCommand("wait");
      app->invalidate();
    }
    break;
  case KeyEvent::KEY_q:
    app->processCommand("quit");
    break;
  case KeyEvent::KEY_p:
    app->processCommand("pick");
    break;
  case KeyEvent::KEY_COMMA:
    if (event.isShiftDown()) {
      app->processCommand("up");
    } else {
      app->processCommand("pick");
    }
    break;
  case KeyEvent::KEY_d:
    if (!event.isShiftDown()) {
      app->modeManager.toDirection();
      app->pendingCommand = DigCommand().aliases.front();
      app->statusLine->setContent({F("Dig: "), State::direction_mode.front()});
    } else {
      app->processCommand("drop");
    }
    break;
  case KeyEvent::KEY_a:
    app->modeManager.toDirection();
    app->pendingCommand = AttackCommand().aliases.front();
    app->statusLine->setContent({F("Attack: "), State::direction_mode.front()});
    break;
  case KeyEvent::KEY_w:
    app->modeManager.toDirection();
    app->pendingCommand = "walk";
    app->statusLine->setContent({F("Walk: "), State::direction_mode.front()});
    break;
  case KeyEvent::KEY_e:
    app->processCommand("equip");
    break;
    break;
  case KeyEvent::KEY_i:
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
  case KeyEvent::KEY_z:
    app->processCommand("zap");
    break;
  case KeyEvent::KEY_r:
    app->hero->currentLocation->leave(app->hero);
    app->hero->currentLocation = app->generator->getRandomLocation(app->hero);
    app->hero->currentLocation->enter(app->hero,
                                      app->hero->currentLocation->enterCell);
    app->state->fragments.assign(
        app->hero->currentLocation->cells.size() *
            (app->hero->currentLocation->cells.front().size() + 1),
        std::make_shared<CellSign>(std::make_shared<Cell>(CellType::UNKNOWN)));
    app->hero->commit("regen location", 0);

    break;
  case KeyEvent::KEY_SLASH:
    if (event.isShiftDown()) {
      app->processCommand("help");
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
    app->processCommand(app->pendingCommand + " " + *dirName);
    return true;
  }
  return false;
}

bool InsertMode::processKey(KeyEvent event) {
  if (event.getCode() != KeyEvent::KEY_SLASH &&
      event.getCode() != KeyEvent::KEY_RETURN &&
      event.getCode() != KeyEvent::KEY_BACKSPACE) {
    app->typedCommand += event.getChar();
  } else if (event.getCode() == KeyEvent::KEY_BACKSPACE) {
    if (app->typedCommand.length() > 0) {
      app->typedCommand.erase(app->typedCommand.length() - 1,
                              app->typedCommand.length());
    } else {
      app->modeManager.toNormal();
      app->statusLine->setContent(State::normal_mode);
      app->typedCommand = "";
      return true;
    }
  } else if (event.getCode() == KeyEvent::KEY_RETURN) {
    app->modeManager.toNormal();
    app->statusLine->setContent(State::normal_mode);
    app->processCommand(app->typedCommand);
    app->typedCommand = "";
    return true;
  }
  if (app->typedCommand.length() == 0) {
    app->statusLine->setContent(State::insert_mode);
  } else {
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

  // TODO: addstandalon end game report
  state->appendContent(
      F(fmt::format("<b>{}</b> [{}]", hero->name, hero->level)));
  state->appendContent(State::END_LINE);
  state->appendContent(F(fmt::format("EXP:<b>{}</b>", hero->exp)));
  state->appendContent(State::END_LINE);
  state->appendContent(F(fmt::format("GOLD:<b>{}</b>", gold)));
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

void ObjectSelectMode::render(std::shared_ptr<State> state) {
  state->setContent({header});
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);

  if (objects.size() == 0) {
    state->appendContent(F("<span color='grey'>Nothing to choose.</span>"));
    return;
  }

  std::string letters = "abcdefghijklmnopqrstuvwxyz";
  auto n = 0;
  for (auto o : objects) {
    state->appendContent(F(formatter(o, std::string{letters[n]})));
    state->appendContent(State::END_LINE);
    n++;
  }
}
