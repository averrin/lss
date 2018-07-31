#include "lss/game/magic.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/lootBox.hpp"
#include "lss/game/terrain.hpp"
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
  if (e.spell == nullptr)
    return;
  auto caster = std::dynamic_pointer_cast<Creature>(e.getSender());
  castSpell(caster, e.spell);
}

void toggleTrait(std::shared_ptr<Player> hero,
                 std::shared_ptr<ToggleTraitSpell> tspell) {
  if (R::R() < tspell->probability) {
    if (hero->hasTrait(tspell->trait)) {
      hero->traits.erase(
          std::remove(hero->traits.begin(), hero->traits.end(), tspell->trait));
    } else {
      hero->traits.push_back(tspell->trait);
    }
  }
  hero->commit("toggle trait", 0);
}

void applyEffect(std::shared_ptr<Player> hero,
                 std::shared_ptr<EffectSpell> espell) {
  hero->activeEffects.push_back(espell->effect);
  hero->commit("apply effect", 0);
}
void heal(std::shared_ptr<Player> hero, int min, int max) {
  auto heal = R::Z(hero->HP_MAX(hero.get()) / 100 * min,
                   hero->HP_MAX(hero.get()) / 100 * max);
  hero->hp += heal;
  if (hero->HP(hero.get()) > hero->HP_MAX(hero.get())) {
    hero->hp = hero->HP_MAX(hero.get());
  }
  hero->commit("heal", 0);
}

void Magic::castSpell(std::shared_ptr<Creature> caster,
                      std::shared_ptr<Spell> spell) {
  if (*spell == *Spells::REVEAL) {
    hero->currentLocation->reveal();
    hero->monsterSense = true;
    hero->commit("reveal", 0);
    hero->monsterSense = false;
    // TODO: move monster sense to toggle trait
  } else if (*spell == *Spells::MONSTER_SENSE) {
    hero->monsterSense = !hero->monsterSense;
  } else if (*spell == *Spells::IDENTIFY) {
    for (auto i : hero->inventory) {
      hero->identify(i);
    }

  } else if (*spell == *Spells::SUMMON_THING) {
    auto c = hero->currentLocation
                 ->cells[hero->currentCell->y + 1][hero->currentCell->x];
    // auto item = Prototype::GOD_PLATE->roll();
    auto lt = LootBox{1, {Prototype::POTION_MANA}};
    auto items = lt.open();
    items.front()->currentCell = c;
    hero->currentLocation->objects.push_back(items.front());
    hero->commit("summon thing", 0);
  } else if (*spell == *Spells::HEAL_LESSER) {
    heal(hero, 10, 25);
  } else if (*spell == *Spells::HEAL) {
    heal(hero, 25, 50);
  } else if (*spell == *Spells::HEAL_GREATER) {
    heal(hero, 50, 100);
  } else if (*spell == *Spells::RESTORE_MANA) {
    auto heal = R::Z(hero->MP_MAX(hero.get()) / 100 * 25,
                     hero->MP_MAX(hero.get()) / 100 * 50);
    hero->mp += heal;
    if (hero->MP(hero.get()) > hero->MP_MAX(hero.get())) {
      hero->mp = hero->MP_MAX(hero.get());
    }
    hero->commit("mana", 0);
  } else if (*spell == *Spells::TELEPORT_RANDOM) {
    auto room = hero->currentLocation
                    ->rooms[rand() % hero->currentLocation->rooms.size()];
    auto cell = room->cells[rand() % room->cells.size()];
    hero->currentCell = cell;
    hero->commit("Teleport", 0);
  } else if (auto tspell = std::dynamic_pointer_cast<ToggleTraitSpell>(spell)) {
    toggleTrait(hero, tspell);
  } else if (auto espell = std::dynamic_pointer_cast<EffectSpell>(spell)) {
    applyEffect(hero, espell);
  } else if (auto rspell = std::dynamic_pointer_cast<RadiusSpell>(spell)) {
    auto cells = caster->getInRadius(rspell->radius);
    applySpellOnCells(rspell->spell, cells);
  } else if (auto lspell = std::dynamic_pointer_cast<LineSpell>(spell)) {
    DirectionEvent de([=](auto dir) {
      auto cell = hero->currentLocation->getCell(caster->currentCell, dir);
      auto cells = std::vector<std::shared_ptr<Cell>>{cell};
      for (auto n=0; n < lspell->length-1; n++) {
        cell = hero->currentLocation->getCell(cell, dir);
        if (!cell->passThrough) break;
        cells.push_back(cell);
      }
      applySpellOnCells(lspell->spell, cells);

    });
    eb::EventBus::FireEvent(de);
  } else if (auto tspell = std::dynamic_pointer_cast<TargetSpell>(spell)) {

    auto cells = caster->getInRadius(tspell->length);
    std::vector<std::shared_ptr<Enemy>> targets;
    for (auto c : cells) {
      auto enemies = utils::castObjects<Enemy>(hero->currentLocation->getObjects(c));
      if (enemies.size() != 0) {
        targets.push_back(enemies.front());
      }
    }
    auto d = tspell->length;
    std::shared_ptr<Cell> target;
    auto cc = hero->currentCell;
    for (auto e : targets) {
      auto td = sqrt(pow(cc->x - e->currentCell->x, 2) + pow(cc->y - e->currentCell->y, 2));
      if (td <= d) {
        target = e->currentCell;
      }
    }

    TargetEvent de(target, [=](auto cell) {
      std::dynamic_pointer_cast<CellSpell>(tspell->spell)->applySpell(hero->currentLocation, cell);
      hero->currentLocation->invalidateVisibilityCache(hero->currentCell);
      hero->calcViewField();
      hero->commit("cast spell", 0);
      pauseAndEraseFireballs();
    }, [=](auto line){
      auto cell = line.back();
      if (!cell->type.passThrough) return false;
      if (line.size() > tspell->length + 1) return false;
      if (std::find_if(line.begin(), line.end(), [](auto c){
        return !c->type.passThrough;
      }) != line.end()) return false;
      return true;
    });
    eb::EventBus::FireEvent(de);
  }
}

void Magic::applySpellOnCells(std::shared_ptr<Spell> spell, std::vector<std::shared_ptr<Cell>> cells) {
      if (auto ds = std::dynamic_pointer_cast<CellSpell>(spell)) {
        for (auto c : cells) {
          ds->applySpell(hero->currentLocation, c);
        }
      }
      hero->currentLocation->invalidateVisibilityCache(hero->currentCell);
      hero->calcViewField();
      hero->commit("cast spell", 0);
      pauseAndEraseFireballs();
}

void Magic::pauseAndEraseFireballs() {
    PauseEvent me([=]() {
      hero->commit("cast spell", 1);

      hero->currentLocation->invalidateVisibilityCache(hero->currentCell);
      hero->currentLocation->invalidate();
      hero->calcViewField();
      hero->commit("cast spell", 0);
    });
    eb::EventBus::FireEvent(me);
}

void DamageSpell::applySpell(std::shared_ptr<Location> location,
                             std::shared_ptr<Cell> c) {
  auto objects = location->getObjects(c);
  for (auto o : objects) {
    if (auto creature = std::dynamic_pointer_cast<Creature>(o)) {
      creature->applyDamage(location->player, damage.getDamage());
    } else {
      location->objects.erase(
          std::remove(location->objects.begin(), location->objects.end(), o),
          location->objects.end());
    }
  }
  applyEffect(location, c);
}
