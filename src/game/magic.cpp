#include "lss/game/magic.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/lootBox.hpp"
#include "lss/generator/room.hpp"
#include "lss/utils.hpp"

Magic::Magic(std::shared_ptr<Player> h) : hero(h) {
    eb::EventBus::AddHandler<ZapCommandEvent>(*this);
}
Magic::~Magic() {}

std::shared_ptr<Enemy> mkEnemy(std::shared_ptr<Location> location,
                               std::shared_ptr<Cell> c,
                               std::shared_ptr<Player> hero, EnemySpec type) {
  auto enemy = std::make_shared<Enemy>(type);
  enemy->currentCell = c;
  enemy->currentLocation = location;

  enemy->handlers.push_back(
      eb::EventBus::AddHandler<CommitEvent>(*enemy, hero));
  enemy->calcViewField();
  return enemy;
}

void Magic::onEvent(ZapCommandEvent &e) {
  if (e.spell == nullptr) return;
  castSpell(e.spell);
}


void Magic::castSpell(std::shared_ptr<Spell> spell) {
  if (*spell == *Spells::REVEAL) {
    hero->currentLocation->reveal();
    hero->monsterSense = true;
    hero->commit("reveal", 0);
    hero->monsterSense = false;
  } else if (*spell == *Spells::MONSTER_SENSE) {
    hero->monsterSense = !hero->monsterSense;
    // } else if (*spell == *Spells::MONSTER_FREEZE) {
    //   for (auto o : hero->currentLocation->objects) {
    //     if (auto e = std::dynamic_pointer_cast<Enemy>(o)) {
    //       e->type.aiType = AIType::NO_AI;
    //     }
    //   }
  } else if (*spell == *Spells::SUMMON_ORK) {
    auto c = hero->currentLocation->cells[hero->currentCell->y + 1]
                                              [hero->currentCell->x];
    hero->currentLocation->objects.push_back(
        mkEnemy(hero->currentLocation, c, hero, EnemyType::ORK));
    hero->commit("summon ork", 0);
  } else if (*spell == *Spells::IDENTIFY) {
    for (auto i : hero->inventory) {
      hero->identify(i);
    }

    MessageEvent me(nullptr, "Your inventory was identified");
    eb::EventBus::FireEvent(me);
  } else if (*spell == *Spells::SUMMON_PLATE) {
    auto c = hero->currentLocation->cells[hero->currentCell->y + 1]
                                              [hero->currentCell->x];
    // auto item = Prototype::GOD_PLATE->roll();
    auto lt = LootBox{1, {Prototype::POTION_MANA}};
    auto items = lt.open();
    items.front()->currentCell = c;
    hero->currentLocation->objects.push_back(items.front());
    hero->commit("summon plate", 0);
  } else if (*spell == *Spells::HEAL_LESSER) {
    auto heal = R::Z(hero->HP_MAX(hero.get()) / 100 * 10,
                     hero->HP_MAX(hero.get()) / 100 * 25);
    hero->hp += heal;
    if (hero->HP(hero.get()) > hero->HP_MAX(hero.get())) {
      hero->hp = hero->HP_MAX(hero.get());
    }
    hero->commit("heal", 0);
    MessageEvent me(nullptr, fmt::format("You healed {} hp", heal));
    eb::EventBus::FireEvent(me);
  } else if (*spell == *Spells::HEAL) {
    auto heal = R::Z(hero->HP_MAX(hero.get()) / 100 * 25,
                     hero->HP_MAX(hero.get()) / 100 * 50);
    hero->hp += heal;
    if (hero->HP(hero.get()) > hero->HP_MAX(hero.get())) {
      hero->hp = hero->HP_MAX(hero.get());
    }
    hero->commit("heal", 0);
    MessageEvent me(nullptr, fmt::format("You healed {} hp", heal));
  } else if (*spell == *Spells::HEAL_GREATER) {
    auto heal = R::Z(hero->HP_MAX(hero.get()) / 100 * 50,
                     hero->HP_MAX(hero.get()) / 100 * 100);
    hero->hp += heal;
    if (hero->HP(hero.get()) > hero->HP_MAX(hero.get())) {
      hero->hp = hero->HP_MAX(hero.get());
    }
    hero->commit("heal", 0);
    MessageEvent me(nullptr, fmt::format("You healed {} hp", heal));
  } else if (*spell == *Spells::RESTORE_MANA) {
    auto heal = R::Z(hero->MP_MAX(hero.get()) / 100 * 25,
                     hero->MP_MAX(hero.get()) / 100 * 50);
    hero->mp += heal;
    if (hero->MP(hero.get()) > hero->MP_MAX(hero.get())) {
      hero->mp = hero->MP_MAX(hero.get());
    }
    hero->commit("mana", 0);
    MessageEvent me(nullptr, fmt::format("Your {} mp restored", heal));
    eb::EventBus::FireEvent(me);
  } else if (*spell == *Spells::TELEPORT_RANDOM) {
    auto room = hero->currentLocation
                    ->rooms[rand() % hero->currentLocation->rooms.size()];
    auto cell = room->cells[rand() % room->cells.size()];
    hero->currentCell = cell;
    hero->commit("Teleport", 0);
    MessageEvent me(nullptr, fmt::format("You were teleported."));
    eb::EventBus::FireEvent(me);
  } else if (auto tspell = std::dynamic_pointer_cast<ToggleTraitSpell>(spell)) {
    if (R::R() < tspell->probability) {
      if (hero->hasTrait(tspell->trait)) {
        hero->traits.erase(std::remove(
            hero->traits.begin(), hero->traits.end(), tspell->trait));
        MessageEvent me(nullptr, fmt::format("Undo {}", tspell->name));
        eb::EventBus::FireEvent(me);
      } else {
        hero->traits.push_back(tspell->trait);
        MessageEvent me(nullptr, fmt::format("Apply {}", tspell->name));
        eb::EventBus::FireEvent(me);
      }
    } else {
      MessageEvent me(nullptr, fmt::format("Nothing happens"));
      eb::EventBus::FireEvent(me);
    }
    hero->commit("toggle trait", 0);
  } else if (auto espell = std::dynamic_pointer_cast<EffectSpell>(spell)) {

    MessageEvent me(nullptr, fmt::format("Apply {} effect", espell->name));
    eb::EventBus::FireEvent(me);
    hero->activeEffects.push_back(espell->effect);
    hero->commit("apply effect", 0);
  }
}
