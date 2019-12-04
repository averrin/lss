#include <memory>

#include "ui/LSSApp.hpp"
#include "ui/actions.hpp"
#include "ui/modes.hpp"
#include "ui/utils.hpp"
#include "lss/utils.hpp"
#include "lss/generator/room.hpp"
#include "lss/commands.hpp"

#include "lss/game/content/enemies.hpp"
#include "lss/game/content/items.hpp"
#include "lss/game/content/traits.hpp"

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

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

void ModeManager::toHero() {
  state_machine.process_event(EnableModeEvent{Modes::HERO});
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

Mode::Mode(std::shared_ptr<LSSApp> a) : app(a){};

bool HintsMode::processKey(KeyEvent event) { return false; }

bool HeroMode::processKey(KeyEvent event) { return false; }

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
    auto d = ui_utils::getDir(event.getCode());
    if (d == std::nullopt)
      break;
    auto nc = app->hero->currentLocation->getCell(
        app->hero->currentLocation
            ->cells[app->state->cursor.y][app->state->cursor.x],
        *utils::getDirectionByName(*d));
    if (!nc)
      break;
    auto location = app->hero->currentLocation;
    auto line = location->getLine(app->hero->currentCell, *nc);
    if (!checkTarget(line))
      break;

    app->state->selectionClear();
    app->state->setCursor({(*nc)->x, (*nc)->y});
    auto cell = location->cells[app->state->cursor.y][app->state->cursor.x];
    for (auto c : line) {
      app->state->setSelection({{c->x, c->y}, "#6c6d79"});
    }

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
  switch (event.getCode()) {
  case SDL_SCANCODE_PERIOD:
    if (event.isShiftDown()) {
      currentPage++;
      if (currentPage > objects.size() / letters.size()) {
        currentPage = objects.size() / letters.size();
      }
      render(app->objectSelectState);
      return true;
    }
    break;
  case SDL_SCANCODE_COMMA:
    if (event.isShiftDown()) {
      currentPage--;
      if (currentPage < 0) {
        currentPage = 0;
      }
      render(app->objectSelectState);
      return true;
    }
    break;
  default: {
    auto ch = event.getChar();
    if (event.isShiftDown()) {
      ch = toupper(ch);
    }
    auto index = letters.find(ch);
    if (objects.size() > index) {
      if (callback(objects[index + (letters.size() * currentPage)])) {
        currentPage = 0;
        return true;
      }
    }

  } break;
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

bool DirectionMode::processKey(KeyEvent event) {
  std::optional<std::string> dirName = ui_utils::getDir(event.getCode());
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

void HeroMode::render(std::shared_ptr<State> state) {
  state->setContent({F("Your character")});
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);
  auto hero = app->hero;
  state->appendContent(
      F(fmt::format("<b>Name</b>:              {}", hero->name)));
  state->appendContent(State::END_LINE);
  state->appendContent(F(fmt::format(
      "<b>HP</b>:                {:d}/{:d} ({:d})", int(hero->HP(hero.get())),
      int(hero->HP_MAX(hero.get())), int(hero->hp_max * hero->strength))));
  state->appendContent(State::END_LINE);
  state->appendContent(F(fmt::format(
      "<b>MP</b>:                {:d}/{:d} ({:d})", int(hero->MP(hero.get())),
      int(hero->MP_MAX(hero.get())), int(hero->mp_max * hero->intelligence))));
  state->appendContent(State::END_LINE);
  state->appendContent(F(fmt::format("<b>Speed</b>:             {} ({})",
                                     hero->SPEED(hero.get()), hero->speed)));
  state->appendContent(State::END_LINE);
  state->appendContent(F(fmt::format("<b>Defence</b>:           {:d}",
                                     int(hero->DEF(hero.get())))));
  state->appendContent(State::END_LINE);
  state->appendContent(
      F(fmt::format("<b>Damage</b>:            {}", hero->getDmgDesc())));
  state->appendContent(State::END_LINE);

  std::string intLine = "          ";
  std::fill_n(intLine.begin(), int((hero->INTELLIGENCE(hero.get()) - 1) * 10),
              '|');
  std::string strLine = "          ";
  std::fill_n(strLine.begin(), int((hero->STRENGTH(hero.get()) - 1) * 10), '|');
  state->appendContent(F(fmt::format("<b>Strength</b>:          <b>[<span "
                                     "color='{{{{green}}}}'>{}</span>]</b>",
                                     strLine)));
  state->appendContent(State::END_LINE);
  state->appendContent(F(fmt::format(
      "<b>Intelligence</b>:      <b>[<span color='{{{{blue}}}}'>{}</span>]</b>",
      intLine)));
  state->appendContent(State::END_LINE);
  state->appendContent(
      F(fmt::format("<b>Spell level</b>:       {:d}",
                    int((hero->INTELLIGENCE(hero.get()) - 1) * 10))));
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);

  state->appendContent(F(fmt::format("<b>Traits</b>:")));
  state->appendContent(State::END_LINE);
  for (auto t : hero->traits) {
    state->appendContent(F(fmt::format("   • {}", t.name)));
    state->appendContent(State::END_LINE);
  }
  if (hero->traits.size() == 0) {
    state->appendContent(
        F(fmt::format("   <span color='grey'>No traits</span>")));
    state->appendContent(State::END_LINE);
  }
  state->appendContent(State::END_LINE);

  state->appendContent(F(fmt::format("<b>Active effects</b>:")));
  state->appendContent(State::END_LINE);
  for (auto e : hero->activeEffects) {
    state->appendContent(F(fmt::format("   • {}", e->getTitle())));
    state->appendContent(State::END_LINE);
  }
  if (hero->activeEffects.size() == 0) {
    state->appendContent(
        F(fmt::format("   <span color='grey'>No effects</span>")));
    state->appendContent(State::END_LINE);
  }
  state->appendContent(State::END_LINE);

  state->appendContent(
      F(fmt::format("AP commited:     <b>{}</b>", hero->report.apCommited)));
  state->appendContent(State::END_LINE);
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

  state->appendContent(F(fmt::format("<b>{}</b>", hero->name)));
  state->appendContent(State::END_LINE);
  state->appendContent(F(
      fmt::format("DEPTH:           <b>{}</b>", hero->currentLocation->depth)));
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

// TODO: add pages
void InventoryMode::render(std::shared_ptr<State> state) {
  state->setContent({header});
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);
  if (objects.size() == 0) {
    state->appendContent(F("<span color='grey'>Nothing here.</span>"));
    return;
  }

  namespace ic = ItemCategories;
  std::vector<ItemCategory> categories = {
      ic::ARMOR,       ic::WEAPONS, ic::THROWABLE, ic::JEWELLERY,
      ic::CONSUMABLES, ic::LIGHT, ic::USABLE,   ic::MISC};
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
  state->setContent(
      {header, F(fmt::format(" [{}/{}]", currentPage + 1,
                             objects.size() / letters.size() + 1))});
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);

  if (objects.size() == 0) {
    state->appendContent(F("<span color='grey'>Nothing to choose.</span>"));
    return;
  }

  auto offset = currentPage * letters.size();

  for (auto n = 0; n < letters.size(); n++) {
    if (n + offset >= objects.size()) {
      break;
    }
    auto o = objects[n + offset];
    state->appendContent(F(formatter(o, std::string{letters[n]})));
    state->appendContent(State::END_LINE);
  }
}
