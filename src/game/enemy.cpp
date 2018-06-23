#include "lss/game/enemy.hpp"
#include "EventBus.hpp"
#include "lss/game/player.hpp"

Enemy::Enemy(EnemySpec t) : Creature(), type(t) {
  hp = type.baseHP;
  hp_max = type.baseHP;
  speed = type.baseSpeed;
  damage_dices = type.baseDamage_dices;
  damage_edges = type.baseDamage_edges;
  damage_modifier = type.baseDamage_modifier;
}

Enemy::~Enemy() { registration->removeHandler(); }

std::optional<std::shared_ptr<Item>> Enemy::drop() {
  // auto item = std::make_shared<Item>(ItemType::CORPSE);
  if (type.loot.size() == 0)
    return std::nullopt;
  auto item = type.loot.front();
  item->currentCell = currentCell;
  return item;
}

bool Enemy::interact(std::shared_ptr<Object> actor) {
  auto hero = std::dynamic_pointer_cast<Player>(actor);
  auto ptr = shared_from_this();
  if (hp > 0) {
    auto damage = hero->getDamage(shared_from_this());
    hp -= damage;
    EnemyTakeDamageEvent e(ptr, damage);
    eb::EventBus::FireEvent(e);
  }
  if (hp <= 0) {
    passThrough = true;
    EnemyDiedEvent e2(ptr);
    eb::EventBus::FireEvent(e2);
  }
  return hp > 0;
}

void Enemy::onEvent(CommitEvent &e) {
  actionPoints += e.actionPoints;
  auto stepCost = 1000 / speed;

  // fmt::print("{}\n", actionPoints);

  // TODO: solve endless cycle
  auto n = 0;
  while (actionPoints >= stepCost && n != 2) {
    if (!move(cd)) {
      if (cd == Direction::W) {
        cd = Direction::E;
      } else {
        cd = Direction::W;
      }
      n++;
    } else {
      actionPoints -= stepCost;
    }
  }
}

EnemyTakeDamageEvent::EnemyTakeDamageEvent(eb::ObjectPtr s, int d)
    : eb::Event(s), damage(d) {}
EnemyDiedEvent::EnemyDiedEvent(eb::ObjectPtr s) : eb::Event(s) {}
