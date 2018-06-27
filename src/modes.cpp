#include <memory>

#include "lss/LSSApp.hpp"
#include "lss/actions.hpp"
#include "lss/commands.hpp"
#include "lss/modes.hpp"

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
  return false;
}

bool NormalMode::processKey(KeyEvent event) {

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
    app->processCommand("inventory");
    break;
  case KeyEvent::KEY_z:
    app->processCommand("zap");
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

void InventoryMode::render(std::shared_ptr<State> state) {
  state->setContent({header});
  state->appendContent(State::END_LINE);
  state->appendContent(State::END_LINE);
  if (objects.size() == 0) {
    state->appendContent(F("<span color='grey'>Nothing here.</span>"));
    return;
  }

  for (auto o : objects) {
    auto item = std::dynamic_pointer_cast<Item>(o);
    std::vector<std::string> effects;
    if (item->effects.size() != 0) {
      for (auto e : item->effects) {
        effects.push_back(e->getTitle());
      }
    }
    state->appendContent(F(fmt::format("     - {}", item->getFullTitle())));
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
