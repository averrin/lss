#include "lss/ui/heroLine.hpp"
#include "fmt/format.h"
#include "lss/game/events.hpp"
#include "lss/game/player.hpp"
#include "lss/utils.hpp"

HeroLine::HeroLine(std::shared_ptr<State> s) : state(s) {
  eb::EventBus::AddHandler<CommitEvent>(*this);
};

void HeroLine::setContent(Fragments content) { state->setContent(content); };

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

void HeroLine::onEvent(CommitEvent &e) {
  auto hero = std::dynamic_pointer_cast<Player>(e.getSender());
  setContent({F(fmt::format("<b>{}</b>", hero->name)),
              F(fmt::format("   <b>HP</b>:{}/{}   <b>SPD</b>:{}   <b>DMG</b>:{}   <b>DEF</b>:{}", hero->HP(hero.get()),
                            hero->HP_MAX(hero.get()), hero->SPEED(hero.get()), hero->getDmgDesc(), hero->DEF(hero.get()))),
              F(fmt::format("{}", hero->haveLight() ? "   L" : "")),
    });
}
