#include "lss/ui/heroLine.hpp"
#include "lss/game/events.hpp"
#include "lss/game/location.hpp"
#include "lss/game/player.hpp"
#include "lss/utils.hpp"
#include <algorithm>
#include <fmt/format.h>

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
  auto lightDurability = 0;
  if (hero->hasLight()) {
    auto lightSlot = *std::find_if(
        hero->equipment->slots.begin(), hero->equipment->slots.end(),
        [](std::shared_ptr<Slot> s) {
          return s->item != nullptr &&
                 std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                           LIGHT) != s->acceptTypes.end();
        });
    lightDurability = lightSlot->item->durability;
  }
  std::string locationFeatures = "_______";
  if (hero->currentLocation->hasFeature(LocationFeature::TORCHES)) {
    locationFeatures[0] = 'T';
  }
  if (hero->currentLocation->hasFeature(LocationFeature::CAVE_PASSAGE)) {
    locationFeatures[1] = 'C';
  }
  if (hero->currentLocation->hasFeature(LocationFeature::RIVER)) {
    locationFeatures[2] = 'R';
  }
  if (hero->currentLocation->hasFeature(LocationFeature::STATUE)) {
    locationFeatures[3] = 'S';
  }
  if (hero->currentLocation->hasFeature(LocationFeature::ALTAR)) {
    locationFeatures[4] = 'A';
  }
  if (hero->currentLocation->hasFeature(LocationFeature::VOID)) {
    locationFeatures[5] = 'V';
  }
  if (hero->currentLocation->hasFeature(LocationFeature::LAKE)) {
    locationFeatures[6] = 'L';
  }
  setContent({
      F(fmt::format("<b>{}</b> [{}]", hero->name, hero->level)),
      F(fmt::format("   <b>HP</b>:{}/{}   <b>SPD</b>:{}   <b>DMG</b>:{}   "
                    "<b>DEF</b>:{}   <b>EXP</b>:{}",
                    hero->HP(hero.get()), hero->HP_MAX(hero.get()),
                    hero->SPEED(hero.get()), hero->getDmgDesc(),
                    hero->DEF(hero.get()), hero->exp)),
      F(fmt::format("{}", hero->hasLight()
                              ? fmt::format("   L&lt;{}&gt;", lightDurability)
                              : "")),
      F(fmt::format("   <b>P</b>:{}.{} D:{} [{} {}]", hero->currentCell->x,
                    hero->currentCell->y, hero->currentLocation->depth,
                    hero->currentLocation->type.name, locationFeatures)),
  });
}
