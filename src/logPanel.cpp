#include "lss/ui/logPanel.hpp"
#include "lss/game/events.hpp"
#include "lss/utils.hpp"
#include "lss/game/player.hpp"

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

LogPanel::LogPanel(std::shared_ptr<State> s, std::shared_ptr<Player> hero) : state(s) {
  eb::EventBus::AddHandler<ItemTakenEvent>(*this);
  eb::EventBus::AddHandler<ItemsFoundEvent>(*this);
  eb::EventBus::AddHandler<MessageEvent>(*this);
  eb::EventBus::AddHandler<DoorOpenedEvent>(*this);
  eb::EventBus::AddHandler<EnemyDiedEvent>(*this);
  eb::EventBus::AddHandler<EnemyTakeDamageEvent>(*this);
  eb::EventBus::AddHandler<HeroTakeDamageEvent>(*this);

  eb::EventBus::AddHandler<CommitEvent>(*this);
  eb::EventBus::AddHandler<EnterCellEvent>(*this, hero);
  eb::EventBus::AddHandler<LeaveCellEvent>(*this, hero);
  eb::EventBus::AddHandler<DigEvent>(*this, hero);
  eb::EventBus::AddHandler<DropEvent>(*this);

  appendLine({F("Long story short...")});
  appendLine(
      {F("You are here. This is dungeon. You need go down. Deal with it.")});
};

void LogPanel::appendLine(Fragments line) {
  if (lines.size() > 0 && *lines.back()->content.front() == *line.front()) {
    lines.back()->count++;
  } else {
    state->appendContent(line);
    lines.push_back(std::make_shared<LogLine>(line));
  }
    if (lines.size() >= 20) {
        lines.pop_front();
    }
  state->setContent({});
  for (auto line : lines) {
    state->appendContent(line->content);
    if (line->count > 1) {
    state->appendContent(
                        F(fmt::format(" <span color='gray'>x{}</span>", line->count)));
    }
    state->appendContent(State::END_LINE);
  }
}

void LogPanel::onEvent(DoorOpenedEvent &e) {
    appendLine({F(fmt::format("You opened the door"))});
}
void LogPanel::onEvent(EnemyTakeDamageEvent &e) {
  auto enemy = std::dynamic_pointer_cast<Enemy>(e.getSender());
  //TODO: add damage object for info about source and crit
  if (e.damage > 0) {
    appendLine({F(fmt::format("You hit {}: {} dmg", enemy->type.name, e.damage->damage))});
  } else {
    appendLine({F(fmt::format("You miss {}", enemy->type.name))});
  }
}
void LogPanel::onEvent(HeroTakeDamageEvent &e) {
  auto enemy = std::dynamic_pointer_cast<Enemy>(e.getSender());
  if (e.damage->damage > 0) {
    std::string tags = "";
    if (e.damage->traits.size() > 0) {
        std::vector<std::string> traitNames;
        for (auto t : e.damage->traits) {
            traitNames.push_back(t.name);
        }
        tags = fmt::format("&lt;{}&gt;", utils::join(traitNames, ", "));
    }
    appendLine({F(fmt::format("You take <span color='{}'>{}{}</span> dmg from {} {}",
                             e.damage->isCritical ? "red" : "#F7CA88",
                             e.damage->isCritical ? "!" : "",
                            e.damage->damage, enemy->type.name, tags))});
  }else {
      auto name = enemy->type.name;
      name[0] = toupper(name[0]);
    appendLine({F(fmt::format("{} failed to hurt you",
                            name))});
      
  }
}
void LogPanel::onEvent(EnemyDiedEvent &e) {
  auto enemy = std::dynamic_pointer_cast<Enemy>(e.getSender());
      auto name = enemy->type.name;
      name[0] = toupper(name[0]);
    appendLine({F(fmt::format("{} died", name))});
}
void LogPanel::onEvent(ItemTakenEvent &e) {}
void LogPanel::onEvent(ItemsFoundEvent &e) {}
void LogPanel::onEvent(MessageEvent &e) {
    appendLine({F(e.message)});
}
void LogPanel::onEvent(EnterCellEvent &e) {
  if (e.cell->hasFeature(CellFeature::BLOOD)) {
    appendLine({F("The floor is splattered with blood.")});
  }
}
void LogPanel::onEvent(LeaveCellEvent &e) {}
void LogPanel::onEvent(DigEvent &e) {}
void LogPanel::onEvent(DropEvent &e) {}
void LogPanel::onEvent(CommitEvent &e) {}
