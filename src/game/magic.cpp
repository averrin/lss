#include "lss/game/magic.hpp"
#include "lss/game/door.hpp"
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
  enemy->setCurrentCell(c);
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

void toggleTrait(std::shared_ptr<Creature> caster,
                 std::shared_ptr<ToggleTraitSpell> tspell) {
  if (R::R() < tspell->probability) {
    if (caster->hasTrait(tspell->trait)) {
      caster->traits.erase(std::remove(caster->traits.begin(),
                                       caster->traits.end(), tspell->trait));
    } else {
      caster->traits.push_back(tspell->trait);
    }
  }
  if (auto hero = std::dynamic_pointer_cast<Player>(caster)) {
    hero->commit("toggle trait", 0);
  }
}

void applyEffect(std::shared_ptr<Creature> caster,
                 std::shared_ptr<EffectSpell> espell) {
  caster->activeEffects.push_back(espell->effect);
  if (auto hero = std::dynamic_pointer_cast<Player>(caster)) {
    hero->commit("apply effect", 0);
  }
}
void Magic::castSpell(std::shared_ptr<Creature> caster,
                      std::shared_ptr<Spell> spell) {
  if (*spell == *Spells::REVEAL) {
    hero->currentLocation->reveal();
    hero->monsterSense = true;
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
    items.front()->setCurrentCell(c);
    hero->currentLocation->addObject(items.front());
  } else if (*spell == *Spells::HEAL_LESSER) {
    caster->heal(10, 25);
  } else if (*spell == *Spells::HEAL) {
    caster->heal(25, 50);
  } else if (*spell == *Spells::HEAL_GREATER) {
    caster->heal(50, 100);
  } else if (*spell == *Spells::RESTORE_MANA) {
    caster->restoreMana(25, 50);
  } else if (*spell == *Spells::TELEPORT_RANDOM) {
    auto room = caster->currentLocation
                    ->rooms[rand() % caster->currentLocation->rooms.size()];
    auto cell = room->cells[rand() % room->cells.size()];
    caster->setCurrentCell(cell);
  } else if (auto tspell = std::dynamic_pointer_cast<ToggleTraitSpell>(spell)) {
    toggleTrait(caster, tspell);
  } else if (auto espell = std::dynamic_pointer_cast<EffectSpell>(spell)) {
    applyEffect(caster, espell);
  } else if (auto rspell = std::dynamic_pointer_cast<RadiusSpell>(spell)) {
    auto cells = caster->getInRadius(rspell->radius);
    applySpellOnCells(caster, rspell->spell, cells);
  } else if (auto lspell = std::dynamic_pointer_cast<LineSpell>(spell)) {
    castLineSpell(caster, lspell);
  } else if (auto tspell = std::dynamic_pointer_cast<TargetSpell>(spell)) {
    castTargetSpell(caster, tspell);
  }
  if (auto hero = std::dynamic_pointer_cast<Player>(caster)) {
    hero->commit("spell effect", 0);
  }
}

void Magic::castLineSpell(std::shared_ptr<Creature> caster,
                          std::shared_ptr<LineSpell> lspell) {
  DirectionEvent de([=](auto dir) {
    auto cell = hero->currentLocation->getCell(caster->currentCell, dir);
    if (!cell)
      return;

    auto cells = std::vector<std::shared_ptr<Cell>>{*cell};
    for (auto n = 0; n < lspell->length - 1; n++) {
      cell = hero->currentLocation->getCell(*cell, dir);
      if (!cell || (!(*cell)->passThrough &&
                    !std::dynamic_pointer_cast<DrillSpell>(lspell->spell)))
        break;
      cells.push_back(*cell);
    }
    applySpellOnCells(caster, lspell->spell, cells);
  });
  eb::EventBus::FireEvent(de);
}

void Magic::castTargetSpell(std::shared_ptr<Creature> caster,
                            std::shared_ptr<TargetSpell> tspell) {
  auto nearestTarget = caster->getNearestTarget(tspell->length);
  std::shared_ptr<Cell> target;
  if (nearestTarget) {
    auto d = hero->currentLocation->getDistance((*nearestTarget)->currentCell,
                                                caster->currentCell);
    if (d <= tspell->length) {
      target = (*nearestTarget)->currentCell;
    }
  }

  TargetEvent de(
      target,
      [=](auto cell) {
        auto spell = std::dynamic_pointer_cast<CellSpell>(tspell->spell);
        auto fb = std::make_shared<Terrain>(spell->spec, 8);
        fb->setCurrentCell(hero->currentCell);
        hero->currentLocation->addObject(fb);
        auto cells = hero->currentLocation->getLine(hero->currentCell, cell);
        auto a = std::make_shared<MoveAnimation>(fb, cells, cells.size());
        a->animationCallback = [=]() {
          spell->applySpell(caster, hero->currentLocation, cell);
          pauseAndEraseFireballs();
        };
        AnimationEvent ae(a);
        eb::EventBus::FireEvent(ae);
      },
      [=](auto line) {
        auto cell = line.back();
        if (!cell->type.passThrough)
          return false;
        if (line.size() > tspell->length + 1)
          return false;
        if (std::find_if(line.begin(), line.end(), [&](auto c) {
              return !c->type.passThrough ||
                     utils::castObjects<Door>(
                         hero->currentLocation->getObjects(c))
                             .size() != 0;
            }) != line.end())
          return false;
        return true;
      });
  eb::EventBus::FireEvent(de);
}

void Magic::applySpellOnCells(std::shared_ptr<Creature> caster,
                              std::shared_ptr<Spell> spell,
                              std::vector<std::shared_ptr<Cell>> cells) {
  if (auto ds = std::dynamic_pointer_cast<CellSpell>(spell)) {
    for (auto c : cells) {
      ds->applySpell(caster, caster->currentLocation, c);
    }
  }
  caster->currentLocation->invalidateVisibilityCache(caster->currentCell);
  caster->calcViewField();
  if (auto hero = std::dynamic_pointer_cast<Player>(caster)) {
    hero->commit("cast spell", 0);
  }
  pauseAndEraseFireballs();
}

void Magic::pauseAndEraseFireballs() {
  PauseEvent me([=]() {
    hero->commit("cast spell", 1);

    hero->currentLocation->invalidateVisibilityCache(hero->currentCell);
    hero->currentLocation->invalidate("cast spell");
    hero->calcViewField();
    hero->commitWaited("cast spell");
  });
  eb::EventBus::FireEvent(me);
}

void DamageSpell::applySpell(std::shared_ptr<Creature> caster,
                             std::shared_ptr<Location> location,
                             std::shared_ptr<Cell> c) {
  auto objects = location->getObjects(c);
  for (auto o : objects) {
    if (auto creature = std::dynamic_pointer_cast<Creature>(o)) {
      creature->applyDamage(
          std::dynamic_pointer_cast<Object>(location->player),
          damage.getDamage(caster->INTELLIGENCE(caster.get())));
    } else if (o->destructable && destroyObjects) {
      location->removeObject(o);
    }
  }
  if (c == location->player->currentCell) {
    location->player->applyDamage(
        std::dynamic_pointer_cast<Object>(location->player),
        damage.getDamage(caster->INTELLIGENCE(caster.get())));
  }
  applyEffect(location, c);
}

void DrillSpell::applySpell(std::shared_ptr<Creature> caster,
                            std::shared_ptr<Location> location,
                            std::shared_ptr<Cell> c) {
  auto objects = location->getObjects(c);
  for (auto o : objects) {
    location->removeObject(o);
  }
  DigEvent de(caster, c);
  eb::EventBus::FireEvent(de);
  applyEffect(location, c);
}

AnimationEvent::AnimationEvent(std::shared_ptr<Animation> a)
    : eb::Event(nullptr), animation(a) {}
