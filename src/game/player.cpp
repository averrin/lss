#include "lss/game/player.hpp"
#include "EventBus.hpp"
#include "lss/commands.hpp"
#include "lss/game/costs.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/slot.hpp"
#include "lss/game/spell.hpp"
#include <memory>

CommitEvent::CommitEvent(eb::ObjectPtr s, int ap)
    : eb::Event(s), actionPoints(ap) {}
DigEvent::DigEvent(eb::ObjectPtr s, std::shared_ptr<Cell> c)
    : eb::Event(s), cell(c) {}
HeroDiedEvent::HeroDiedEvent(eb::ObjectPtr s) : eb::Event(s) {}

HeroTakeDamageEvent::HeroTakeDamageEvent(eb::ObjectPtr s, int d)
    : eb::Event(s), damage(d) {}

Player::Player() : Creature() {
  eb::EventBus::AddHandler<MoveCommandEvent>(*this);
  eb::EventBus::AddHandler<WalkCommandEvent>(*this);
  eb::EventBus::AddHandler<PickCommandEvent>(*this);
  eb::EventBus::AddHandler<DigCommandEvent>(*this);
  eb::EventBus::AddHandler<AttackCommandEvent>(*this);
  eb::EventBus::AddHandler<EquipCommandEvent>(*this);
  eb::EventBus::AddHandler<UnEquipCommandEvent>(*this);
  eb::EventBus::AddHandler<DropCommandEvent>(*this);
  eb::EventBus::AddHandler<WaitCommandEvent>(*this);
  eb::EventBus::AddHandler<ZapCommandEvent>(*this);

  equipment = std::make_shared<Equipment>();
  auto right_hand_slot = std::make_shared<Slot>(
      "Right hand", std::vector<WearableType>{WEAPON, WEAPON_LIGHT,
                                              WEAPON_TWOHANDED, SHIELD});
  auto left_hand_slot = std::make_shared<Slot>(
      "Left hand", std::vector<WearableType>{WEAPON_LIGHT, SHIELD});
  equipment->slots = {
      std::make_shared<Slot>("Head", std::vector<WearableType>{HEAD}),
      std::make_shared<Slot>("Neck", std::vector<WearableType>{NECK}),
      std::make_shared<Slot>("Cloak", std::vector<WearableType>{CLOAK}),
      std::make_shared<Slot>("Right pauldron",
                             std::vector<WearableType>{RIGHT_PAULDRON}),
      std::make_shared<Slot>("Left pauldron",
                             std::vector<WearableType>{LEFT_PAULDRON}),
      right_hand_slot,
      left_hand_slot,
      std::make_shared<Slot>("Right gauntlet",
                             std::vector<WearableType>{RIGHT_GAUNTLET}),
      std::make_shared<Slot>("Left gauntlet",
                             std::vector<WearableType>{LEFT_GAUNTLET}),
      std::make_shared<Slot>("Right ring", std::vector<WearableType>{RING}),
      std::make_shared<Slot>("Left ring", std::vector<WearableType>{RING}),
      std::make_shared<Slot>("Body", std::vector<WearableType>{BODY}),
      std::make_shared<Slot>("Greaves", std::vector<WearableType>{GREAVES}),
      std::make_shared<Slot>("Boots", std::vector<WearableType>{BOOTS}),
      std::make_shared<Slot>("Light", std::vector<WearableType>{LIGHT})};

  hp = 20;
  hp_max = 20;
  damage_dices = 1;
  damage_edges = 1;
  damage_modifier = 0;
  visibilityDistance = 3.2f;

  name = "Unnamed hero";

  auto dagger = std::make_shared<Item>(
      ItemType::DAGGER, Effects{std::make_shared<MeleeDamage>(1, 2, 2)});
  inventory.push_back(dagger);
  auto sword = std::make_shared<Item>(
      ItemType::SWORD, Effects{std::make_shared<MeleeDamage>(2, 4, 3)});
  inventory.push_back(sword);
  inventory.push_back(Prototype::TORCH->clone());
  inventory.push_back(Prototype::PLATE->clone());

  auto axe = std::make_shared<Item>(
      ItemType::GREAT_AXE, Effects{std::make_shared<MeleeDamage>(-1, 6, 7),
                                   std::make_shared<SpeedModifier>(-0.3f)});
  inventory.push_back(axe);
}

std::string Player::getDmgDesc() {
  auto primaryDmg = getPrimaryDmg();

  auto haveLeft =
      std::count_if(
          equipment->slots.begin(), equipment->slots.end(),
          [](std::shared_ptr<Slot> s) {
            return s->item != nullptr &&
                   s->item->type.wearableType !=
                       WearableType::WEAPON_TWOHANDED &&
                   std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                             WEAPON_LIGHT) != s->acceptTypes.end() &&
                   std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                             WEAPON) == s->acceptTypes.end();
          }) > 0;

  if (primaryDmg != std::nullopt && haveLeft) {
    auto [primarySlot, m, d, e] = *primaryDmg;
    auto secondaryDmg = getSecondaryDmg(primarySlot);
    if (secondaryDmg != std::nullopt) {
      auto [secondarySlot, m2, d2, e2] = *secondaryDmg;
      return fmt::format("{:+d} {}d{}{}", m, d, e,
                         hasTrait(Traits::DUAL_WIELD)
                             ? fmt::format(" ({:+d} {}d{})", m2, d2, e2)
                             : fmt::format(" ({:+d})", m2));
    }
  } else if (haveLeft) {
    auto secondaryDmg = getSecondaryDmg(nullptr);
    if (secondaryDmg != std::nullopt) {
      auto [secondarySlot, m2, d2, e2] = *secondaryDmg;
      return hasTrait(Traits::DUAL_WIELD)
                 ? fmt::format("~ {:+d} {}d{}", m2, d2, e2)
                 : fmt::format("~ {:+d}", m2);
    }
  } else if (primaryDmg != std::nullopt) {
    auto [primarySlot, m, d, e] = *primaryDmg;
    return fmt::format("{:+d} {}d{}", m, d, e);
  }
  return fmt::format("{:+d} {}d{}", damage_modifier, damage_dices,
                     damage_edges);
}

void Player::commit(int ap) {
  if (hasLight() && !hasTrait(Traits::MAGIC_TORCH)) {
    auto lightSlot = *std::find_if(
        equipment->slots.begin(), equipment->slots.end(),
        [](std::shared_ptr<Slot> s) {
          return s->item != nullptr &&
                 std::find(s->acceptTypes.begin(), s->acceptTypes.end(),
                           LIGHT) != s->acceptTypes.end();
        });
    if (lightSlot->item != nullptr && lightSlot->item->durability != -1) {
      lightSlot->item->durability -= ap / 10;
      if (lightSlot->item->durability <= 0) {
        lightSlot->item->durability = 0;
        equipment->unequip(lightSlot);
      }
    }
  }

  auto ptr = shared_from_this();
  CommitEvent e(ptr, ap);
  eb::EventBus::FireEvent(e);
}

bool Player::unequip(std::shared_ptr<Slot> slot) {
  equipment->unequip(slot);
  commit(ap_cost::UNEQUIP / SPEED(this));
  return true;
}

bool Player::equip(std::shared_ptr<Slot> slot, std::shared_ptr<Item> item) {
  // auto ptr = shared_from_this();
  if (equipment->equip(slot, item)) {
    commit(ap_cost::EQUIP / SPEED(this));
    return true;
  }
  return false;

  // ItemTakenEvent e(ptr, item);
  // eb::EventBus::FireEvent(e);
}

void Player::onEvent(MoveCommandEvent &e) {
  if (move(e.direction, true)) {
    commit(ap_cost::STEP / SPEED(this));
  } else {
    commit(0);
  }
}

void Player::onEvent(AttackCommandEvent &e) {
  if (attack(e.direction)) {
    commit(ap_cost::ATTACK / SPEED(this));
  }
}

void Player::onEvent(DropCommandEvent &e) {
  if (e.item == nullptr)
    return;
  drop(e.item);
  commit(ap_cost::DROP / SPEED(this));
}

void Player::onEvent(DigCommandEvent &e) {
  if (std::find_if(inventory.begin(), inventory.end(),
                   [](std::shared_ptr<Item> item) {
                     return item->type == ItemType::PICK_AXE && item->equipped;
                   }) == inventory.end()) {
    MessageEvent me(shared_from_this(), "You need to equip pick axe");
    eb::EventBus::FireEvent(me);
    return;
  }

  auto cell = getCell(e.direction);

  DigEvent de(shared_from_this(), cell);
  eb::EventBus::FireEvent(de);
}

void Player::onEvent(WalkCommandEvent &e) {
  while (move(e.direction)) {
    commit(ap_cost::STEP / SPEED(this));
    auto item = std::find_if(currentLocation->objects.begin(),
                             currentLocation->objects.end(),
                             [&](std::shared_ptr<Object> o) {
                               return std::dynamic_pointer_cast<Item>(o) &&
                                      o->currentCell == currentCell;
                             });
    if (item != currentLocation->objects.end())
      break;
  }
}

void Player::onEvent(PickCommandEvent &e) {
  auto item = std::find_if(
      currentLocation->objects.begin(), currentLocation->objects.end(),
      [&](std::shared_ptr<Object> o) {
        return (std::dynamic_pointer_cast<Item>(o) ||
                std::dynamic_pointer_cast<TorchStand>(o)) &&
               o->currentCell == currentCell;
      });
  if (item != currentLocation->objects.end()) {
    if (std::dynamic_pointer_cast<TorchStand>(*item)) {
      pick(Prototype::TORCH->clone());
      currentLocation->objects.erase(
          std::remove(currentLocation->objects.begin(),
                      currentLocation->objects.end(), *item),
          currentLocation->objects.end());
    } else {
      pick(std::dynamic_pointer_cast<Item>(*item));
    }
    commit(ap_cost::PICK / SPEED(this));
  }
}

void Player::onEvent(EquipCommandEvent &e) {
  if (e.item == nullptr)
    return;
  fmt::print("Equipment equip: {} --> {}\n", e.item->type.name, e.slot->name);
  equip(e.slot, e.item);
}

void Player::onEvent(UnEquipCommandEvent &e) { unequip(e.slot); }
void Player::onEvent(WaitCommandEvent &e) { commit(ap_cost::STEP); }
void Player::onEvent(ZapCommandEvent &e) {
  if (e.spell == nullptr)
    return;
  commit(e.spell->cost);
}

bool Player::interact(std::shared_ptr<Object> actor) {
  auto enemy = std::dynamic_pointer_cast<Enemy>(actor);
  auto ptr = shared_from_this();
  if (hp > 0) {
    auto damage = enemy->getDamage(shared_from_this());
    auto def = DEF(this);
    if (damage - def < 0) {
      damage = 0;
    } else {
      damage -= def;
    }
    hp -= damage;
    HeroTakeDamageEvent e(ptr, damage);
    eb::EventBus::FireEvent(e);
  }
  if (hp <= 0) {
    passThrough = true;
    HeroDiedEvent e2(ptr);
    // MessageEvent e2(ptr, "YOU DIED!!");
    eb::EventBus::FireEvent(e2);
  }
  commit(0);
  return hp > 0;
}
