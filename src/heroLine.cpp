#include "lss/ui/heroLine.hpp"
#include "lss/game/events.hpp"
#include "lss/game/location.hpp"
#include "lss/game/player.hpp"
#include "lss/utils.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <string>

using namespace std::string_literals;

HeroLine::HeroLine(std::shared_ptr<State> s, std::shared_ptr<Player> h)
    : state(s), hero(h) {
  handlers.push_back(eb::EventBus::AddHandler<CommitEvent>(*this));
  handlers.push_back(eb::EventBus::AddHandler<HeroTakeDamageEvent>(*this));
};

HeroLine::~HeroLine() {
  for (auto r : handlers) {
    r->removeHandler();
  }
}

void HeroLine::setContent(Fragments content) { state->setContent(content); };

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

void HeroLine::onEvent(CommitEvent &e) { update(); }
void HeroLine::onEvent(HeroTakeDamageEvent &e) { update(); }

void HeroLine::update() {
  float lightDurability = 0;
  if (hero->hasLight()) {
    auto lightSlot = *std::find_if(
        hero->equipment->slots.begin(), hero->equipment->slots.end(),
        [](std::shared_ptr<Slot> s) {
          return s->item != nullptr &&
                 std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                           LIGHT) != s->acceptTypes.end();
        });
    lightDurability =
        float(lightSlot->item->durability) / lightSlot->item->type.durability;
  }
  auto locationFeatures = hero->currentLocation->getFeaturesTag();
  std::string effects = "";
  for_each(hero->activeEffects.begin(), hero->activeEffects.end(),
           [&](auto e) { effects.append(e->getSign()); });

  std::string healthLine = "        ";
  int h =
      hero->HP(hero.get()) / int(hero->HP_MAX(hero.get())) * healthLine.size();
  std::fill_n(healthLine.begin(), h, '|');

  std::string manaLine = "        ";
  int m =
      hero->MP(hero.get()) / int(hero->MP_MAX(hero.get())) * manaLine.size();
  std::fill_n(manaLine.begin(), m, '|');

  std::string ldLine = "        ";
  int l = lightDurability * ldLine.size();
  std::fill_n(ldLine.begin(), l, '|');

  setContent(
      {F(fmt::format("<b>{}</b> [{}]   {}", hero->name, hero->level, effects)),
       F(fmt::format("   <b>HP:[<span color='{{{{green}}}}'>{}</span>]</b>"
                     "   <b>MP:[<span color='{{{{blue}}}}'>{}</span>]</b>    "
                     "<b>DMG</b>:{}   "
                     "<b>DEF</b>:{}",
                     healthLine, manaLine, hero->getDmgDesc(),
                     hero->DEF(hero.get()))),
       F(fmt::format(
           "{}",
           hero->emitsLight
               ? fmt::format(
                     "   <b>L:[<span color='{{{{orange}}}}'>{}</span>]</b>",
                     ldLine)
               : "")),
       F(fmt::format("   <b>P</b>:{}.{} D:{} [{} {}].{}", hero->currentCell->x,
                     hero->currentCell->y, hero->currentLocation->depth,
                     hero->currentLocation->type.name, locationFeatures,
                     hero->currentLocation->rooms.size())),
       // State::END_LINE.front(),
       F(fmt::format("   <b>I</b>:{:g}", hero->INTELLIGENCE(hero.get()))),
       F(fmt::format("   <b>S</b>:{:g}", hero->STRENGTH(hero.get())))});
}
