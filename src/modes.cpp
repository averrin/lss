#include <memory>

#include "lss/LSSApp.hpp"
#include "lss/actions.hpp"
#include "lss/command.hpp"
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

void ModeManager::toItemSelect() {
  state_machine.process_event(EnableModeEvent{Modes::ITEMSELECT});
}

void HintsMode::processEvent(std::shared_ptr<LssEvent> event) {}

Mode::Mode(LSSApp *a) : app(a){};

bool HintsMode::processKey(KeyEvent event) { return false; }

bool ItemSelectMode::processKey(KeyEvent event) {
    std::string letters = "abcdefghijklmnopqrstuvwxyz";
    auto index = letters.find(event.getChar());
    auto e = std::make_shared<EquipCommandEvent>(app->hero->inventory[index]);
    eb::EventBus::FireEvent(*e);
    app->modeManager.toNormal();
    return false;
}

bool NormalMode::processKey(KeyEvent event) {

  switch (event.getCode()) {
  case KeyEvent::KEY_k:
    app->processCommand("move n");
    break;
  case KeyEvent::KEY_l:
    app->processCommand("e");
    break;
  case KeyEvent::KEY_j:
    app->processCommand("m s");
    break;
  case KeyEvent::KEY_h:
    app->processCommand("w");
    break;
  case KeyEvent::KEY_q:
    app->processCommand("q");
    break;
  case KeyEvent::KEY_p:
    app->processCommand("p");
    break;
  case KeyEvent::KEY_d:
    app->modeManager.toDirection();
    app->pendingCommand = DigCommand().aliases.front();
    app->statusLine->setContent({F("Dig: "), State::direction_mode.front()});
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
