#include <memory>

#include "lss/actions.hpp"
#include "lss/modes.hpp"

ModeManager::ModeManager(){};

void ModeManager::processKey(KeyEvent event) {
  state_machine.process_event(KeyPressedEvent(event));
}

void ModeManager::processEvent(std::shared_ptr<LssEvent> event) {
  state_machine.process_event(*event);
}

void ModeManager::toNormal() {
  state_machine.process_event(ModeExitedEvent{});
}

void ModeManager::toDirection() {
  state_machine.process_event(EnableModeEvent{Modes::DIRECTION});
}

void HintsMode::processEvent(std::shared_ptr<LssEvent> event) {}

Mode::Mode(std::shared_ptr<State> s) : state(s){};

bool HintsMode::processKey(KeyEvent event) { return false; }
