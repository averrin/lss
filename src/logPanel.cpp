#include "lss/ui/logPanel.hpp"
#include "lss/game/events.hpp"
#include "lss/game/player.hpp"
#include "lss/utils.hpp"

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

LogPanel::~LogPanel() {
  for (auto r : handlers) {
    r->removeHandler();
  }
}

LogPanel::LogPanel(std::shared_ptr<State> s, std::shared_ptr<Player> h)
    : state(s), hero(h) {
  handlers.push_back(eb::EventBus::AddHandler<ItemTakenEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<ItemsFoundEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<MessageEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<DoorOpenedEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<EnemyDiedEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<EnemyTakeDamageEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<HeroTakeDamageEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<HeroDiedEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<CommitEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<EnterCellEvent>(*this, hero));
  handlers.push_back(eb::EventBus::AddHandler<LeaveCellEvent>(*this, hero));
  handlers.push_back(eb::EventBus::AddHandler<DigEvent>(*this, hero));
  handlers.push_back(eb::EventBus::AddHandler<DropEvent>(*this));

  appendLine({F("Long story short...")});
  appendLine(
      {F("You are here. This is dungeon. You need go down. Deal with it.")});
  appendLine({F("If you dont know how to roguelike just press [<span "
                "weight='bold' color='{{orange}}'>?</span>] for controls.")});
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
  if (e.damage > 0) {
    appendLine(
        {F(fmt::format("You <b>hit</b> {}: <b>{}</b> dmg{}", enemy->type.name,
                       e.damage->damage,
                       e.damage->deflicted > 0
                           ? fmt::format(" [{} deflicted]", e.damage->deflicted)
                           : ""))});
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
      tags = fmt::format(" &lt;{}&gt;", utils::join(traitNames, ", "));
    }
    appendLine({F(fmt::format(
        "You take <span color='{}'><b>{}{}</b></span> dmg from {}{}{}",
        e.damage->isCritical ? "red" : "{{orange}}",
        e.damage->isCritical ? "!" : "", e.damage->damage, enemy->type.name,
        tags,
        e.damage->deflicted > 0
            ? fmt::format(" [{} deflicted]", e.damage->deflicted)
            : ""))});
  } else {
    auto name = enemy->type.name;
    name[0] = toupper(name[0]);
    appendLine({F(fmt::format("{} failed to hurt you", name))});
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
void LogPanel::onEvent(MessageEvent &e) { appendLine({F(e.message)}); }

// TODO: migrate to one cell related event with src and dest (for handling room
// enter-exit)
void LogPanel::onEvent(EnterCellEvent &e) {
  if (e.cell->hasFeature(CellFeature::BLOOD)) {
    appendLine({F("The floor is splattered with blood.")});
  }
  if (auto t = utils::castObjects<Terrain>(
          hero->currentLocation->getObjects(e.cell));
      t.size() == 1) {
    if (t.front()->type == TerrainType::ALTAR) {
      appendLine({F("There is a dark stone altar.")});
    } else if (t.front()->type == TerrainType::BUSH) {
      appendLine({F("There are dense bushes.")});
    }
  }
}
void LogPanel::onEvent(LeaveCellEvent &e) {}
void LogPanel::onEvent(DigEvent &e) {}
void LogPanel::onEvent(DropEvent &e) {}
void LogPanel::onEvent(CommitEvent &e) {}

void LogPanel::onEvent(HeroDiedEvent &e) { appendLine({F("You died...")}); }