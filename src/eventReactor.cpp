#include "lss/eventReactor.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/spell.hpp"
#include "lss/generator/room.hpp"
#include "lss/state.hpp"
#include "lss/utils.hpp"

std::shared_ptr<Fragment> F(std::string c) {
  return std::make_shared<Fragment>(c);
}

QuitCommandEvent::QuitCommandEvent() : CommandEvent(nullptr) {}
std::optional<std::shared_ptr<CommandEvent>>
QuitCommand::getEvent(std::string s) {
  return std::make_shared<QuitCommandEvent>();
}

void EventReactor::onEvent(StairEvent &e) {
  if (app->hero->currentCell->type == CellType::UPSTAIRS &&
      e.dir == StairType::UP) {
    if (app->currentLevel == 0)
      return;
    app->currentLevel--;
    app->hero->currentLocation->leave(app->hero);
    app->hero->currentLocation = app->locations[app->currentLevel];
    app->hero->currentLocation->enter(app->hero,
                                      app->hero->currentLocation->exitCell);

    app->state->fragments.assign(
        app->hero->currentLocation->cells.size() *
            (app->hero->currentLocation->cells.front().size() + 1),
        std::make_shared<CellSign>(std::make_shared<Cell>(CellType::UNKNOWN)));
    app->hero->commit("up in er", 0);
    app->invalidate("enter");
  } else if (app->hero->currentCell->type == CellType::DOWNSTAIRS &&
             e.dir == StairType::DOWN) {
    if (app->locations.find(app->currentLevel + 1) == app->locations.end()) {
      // app->locations[app->currentLevel+1] =
      // app->generator->getRandomLocation(app->hero);
      fmt::print("Wait for location generation\n");
      app->threads[app->currentLevel].join();
    }

    if (app->currentLevel == 25)
      return;
    app->currentLevel++;
    app->hero->currentLocation->leave(app->hero);
    app->hero->currentLocation = app->locations[app->currentLevel];
    app->hero->currentLocation->enter(app->hero,
                                      app->hero->currentLocation->enterCell);

    app->state->fragments.assign(
        app->hero->currentLocation->cells.size() *
            (app->hero->currentLocation->cells.front().size() + 1),
        std::make_shared<CellSign>(std::make_shared<Cell>(CellType::UNKNOWN)));

    app->hero->currentLocation->updateView(app->hero);
    app->hero->commit("down in er", 0);
  } else {
    MessageEvent me(nullptr, "There is no suitable stair.");
    eb::EventBus::FireEvent(me);
    return;
  }
}

void EventReactor::onEvent(eb::Event &e) { app->invalidate("eb::event"); }

void EventReactor::onEvent(CommitEvent &e) {
  if (e.silent) {
    return;
  }
  app->invalidate("commit");
}

void EventReactor::onEvent(LocationChangeEvent &e) {
  app->invalidate("location change");
}

void EventReactor::onEvent(QuitCommandEvent &e) { app->running = false; }

void EventReactor::onEvent(HelpCommandEvent &e) {
  app->helpMode->setHeader(State::HELP_HEADER.front());
  app->helpMode->render(app->helpState);
  app->modeManager.toHelp();
  app->statusLine->setContent(State::text_mode);
}
void EventReactor::onEvent(InventoryCommandEvent &e) {
  app->inventoryMode->setHeader(State::INVENTORY_HEADER.front());
  auto objects = utils::castObjects<Object>(app->hero->inventory);

  std::sort(objects.begin(), objects.end(),
            [](std::shared_ptr<Object> a, std::shared_ptr<Object> b) {
              return std::dynamic_pointer_cast<Item>(a)->name <
                     std::dynamic_pointer_cast<Item>(b)->name;
            });
  app->inventoryMode->setObjects(objects);
  app->inventoryMode->render(app->inventoryState);
  app->modeManager.toInventory();
  app->statusLine->setContent(State::text_mode);
}

void EventReactor::onEvent(UseCommandEvent &e) {
  if (auto c = std::dynamic_pointer_cast<Consumable>(e.item);
      c && e.item != nullptr) {
    auto spell = c->spell;

    app->hero->identify(e.item);
    std::for_each(app->hero->inventory.begin(), app->hero->inventory.end(),
                  [&](auto item) {
                    if (item->getTitle(true) == e.item->getTitle(true)) {
                      app->hero->identify(item);
                    }
                  });

    app->hero->inventory.erase(std::remove(app->hero->inventory.begin(),
                                           app->hero->inventory.end(), e.item));

    auto me = std::make_shared<MessageEvent>(
        nullptr, fmt::format("You use {}", e.item->getTitle()));
    eb::EventBus::FireEvent(*me);
    return castSpell(spell);
  }
  app->objectSelectMode->setHeader(F("Items to use: "));

  auto usable = utils::castObjects<Consumable>(app->hero->inventory);
  app->objectSelectMode->setObjects(utils::castObjects<Object>(usable));

  Formatter formatter = [](std::shared_ptr<Object> o, std::string letter) {
    if (auto item = std::dynamic_pointer_cast<Consumable>(o)) {
      return fmt::format("<span weight='bold'>{}</span> - {}{}", letter,
                         item->getFullTitle(),
                         item->spell == nullptr ? "*" : "");
    }
    return "Unknown error"s;
  };
  app->objectSelectMode->setFormatter(formatter);

  app->objectSelectMode->setCallback([&](std::shared_ptr<Object> o) {
    auto item = std::dynamic_pointer_cast<Consumable>(o);
    UseCommandEvent e(item);
    eb::EventBus::FireEvent(e);
    app->modeManager.toNormal();
    return true;
  });

  app->objectSelectMode->render(app->objectSelectState);
  app->modeManager.toObjectSelect();
}

void EventReactor::onEvent(DropCommandEvent &e) {
  if (e.item != nullptr)
    return;

  app->objectSelectMode->setHeader(F("Items to drop: "));

  Items dropable(app->hero->inventory.size());
  auto it =
      std::copy_if(app->hero->inventory.begin(), app->hero->inventory.end(),
                   dropable.begin(),
                   [](std::shared_ptr<Item> item) { return !item->equipped; });

  dropable.resize(std::distance(dropable.begin(), it));
  app->objectSelectMode->setObjects(utils::castObjects<Object>(dropable));

  Formatter formatter = [](std::shared_ptr<Object> o, std::string letter) {
    auto item = std::dynamic_pointer_cast<Item>(o);
    return fmt::format("<span weight='bold'>{}</span> - {}", letter,
                       item->getFullTitle());
  };
  app->objectSelectMode->setFormatter(formatter);

  app->objectSelectMode->setCallback([&](std::shared_ptr<Object> o) {
    auto item = std::dynamic_pointer_cast<Item>(o);
    auto e = std::make_shared<DropCommandEvent>(item);
    eb::EventBus::FireEvent(*e);
    app->modeManager.toNormal();
    return true;
  });

  app->objectSelectMode->render(app->objectSelectState);
  app->modeManager.toObjectSelect();
}

bool EventReactor::slotCallback(std::shared_ptr<Object> o) {
  auto slot = std::dynamic_pointer_cast<Slot>(o);
  fmt::print("Selected slot: {}\n", slot->name);

  if (std::find_if(app->hero->inventory.begin(), app->hero->inventory.end(),
                   [slot](std::shared_ptr<Item> item) {
                     return item->type.wearableType != INVALID &&
                            std::find(slot->acceptTypes.begin(),
                                      slot->acceptTypes.end(),
                                      item->type.wearableType) !=
                                slot->acceptTypes.end();
                   }) == app->hero->inventory.end()) {
    return false;
  }

  if (slot->item != nullptr) {
    auto e = std::make_shared<UnEquipCommandEvent>(slot);
    eb::EventBus::FireEvent(*e);
    app->objectSelectMode->render(app->objectSelectState);
    return true;
  }

  app->objectSelectMode->setHeader(F("Items to equip: "));

  Items equipable(app->hero->inventory.size());
  auto it = std::copy_if(
      app->hero->inventory.begin(), app->hero->inventory.end(),
      equipable.begin(), [slot](std::shared_ptr<Item> item) {
        return !item->equipped && item->type.wearableType != INVALID &&
               std::find(slot->acceptTypes.begin(), slot->acceptTypes.end(),
                         item->type.wearableType) != slot->acceptTypes.end();
      });

  equipable.resize(std::distance(equipable.begin(), it));
  app->objectSelectMode->setObjects(utils::castObjects<Object>(equipable));

  Formatter formatter = [](std::shared_ptr<Object> o, std::string letter) {
    auto item = std::dynamic_pointer_cast<Item>(o);
    return fmt::format("<span weight='bold'>{}</span> - {}", letter,
                       item->getFullTitle());
  };
  app->objectSelectMode->setFormatter(formatter);

  app->objectSelectMode->setCallback(
      [=](std::shared_ptr<Object> o) { return itemCallback(slot, o); });

  app->objectSelectMode->render(app->objectSelectState);
  return true;
}

bool EventReactor::itemCallback(std::shared_ptr<Slot> slot,
                                std::shared_ptr<Object> o) {
  auto item = std::dynamic_pointer_cast<Item>(o);

  auto e = std::make_shared<EquipCommandEvent>(slot, item);
  eb::EventBus::FireEvent(*e);

  e = std::make_shared<EquipCommandEvent>();
  eb::EventBus::FireEvent(*e);
  return true;
}

void EventReactor::onEvent(EquipCommandEvent &e) {
  if (e.item != nullptr)
    return;

  app->objectSelectMode->setHeader(F("Select slot: "));
  app->objectSelectMode->setObjects(
      utils::castObjects<Object>(app->hero->equipment->slots));

  Formatter formatter = [&](std::shared_ptr<Object> o, std::string letter) {
    auto slot = std::dynamic_pointer_cast<Slot>(o);
    bool have_items = true;
    auto shortcut = fmt::format("<span weight='bold'>{}</span> -", letter);
    if (std::find_if(app->hero->inventory.begin(), app->hero->inventory.end(),
                     [slot](std::shared_ptr<Item> item) {
                       return item->type.wearableType != INVALID &&
                              item->durability != 0 &&
                              std::find(slot->acceptTypes.begin(),
                                        slot->acceptTypes.end(),
                                        item->type.wearableType) !=
                                  slot->acceptTypes.end();
                     }) == app->hero->inventory.end()) {
      shortcut = "   ";
      have_items = false;
    }
    auto fromThisSlot =
        slot->item != nullptr &&
        std::find(slot->acceptTypes.begin(), slot->acceptTypes.end(),
                  slot->item->type.wearableType) != slot->acceptTypes.end();

    return fmt::format(
        "<span color='{}'>{} {:16} :</span> {}",
        have_items ? "{{orange}}" : "gray", shortcut, slot->name,
        slot->item == nullptr
            ? (have_items ? "-" : "<span color='gray'>-</span>")
            : (fromThisSlot
                   ? slot->item->getTitle()
                   : "<span color='gray'>item from other slot</span>"));
  };
  app->objectSelectMode->setFormatter(formatter);

  app->objectSelectMode->setCallback(
      [&](std::shared_ptr<Object> o) { return slotCallback(o); });

  app->objectSelectMode->render(app->objectSelectState);

  app->modeManager.toObjectSelect();
  app->statusLine->setContent(State::object_select_mode);
}

void EventReactor::onEvent(ZapCommandEvent &e) {
  if (e.spell != nullptr)
    return castSpell(e.spell);

  app->objectSelectMode->setHeader(F("Spells for zap: "));

  if (app->debug) {
    app->objectSelectMode->setObjects(
        utils::castObjects<Object>(Spells::USABLE));
  } else {
    std::vector<std::shared_ptr<Spell>> spells(Spells::USABLE.size());
    auto it = std::copy_if(Spells::USABLE.begin(), Spells::USABLE.end(),
                           spells.begin(),
                           [](auto spell) { return spell->cost != 0; });

    spells.resize(std::distance(spells.begin(), it));

    app->objectSelectMode->setObjects(utils::castObjects<Object>(spells));
  }

  Formatter formatter = [&](std::shared_ptr<Object> o, std::string letter) {
    if (auto spell = std::dynamic_pointer_cast<Spell>(o)) {
      auto castable = spell->cost <= app->hero->MP(app->hero.get());
      return fmt::format(
          "<span{}><span weight='bold'>{}</span> - {:32} {}</span>",
          castable ? "" : " color='gray'", letter, spell->name, spell->cost);
    }
    return "Unknown error"s;
  };
  app->objectSelectMode->setFormatter(formatter);

  app->objectSelectMode->setCallback([&](std::shared_ptr<Object> o) {
    auto spell = std::dynamic_pointer_cast<Spell>(o);
    auto castable = spell->cost <= app->hero->MP(app->hero.get());
    if (!castable)
      return false;
    auto e = std::make_shared<ZapCommandEvent>(spell);
    eb::EventBus::FireEvent(*e);
    app->modeManager.toNormal();
    return true;
  });

  app->objectSelectMode->render(app->objectSelectState);
  app->modeManager.toObjectSelect();
  app->statusLine->setContent(State::text_mode);
}

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

void EventReactor::castSpell(std::shared_ptr<Spell> spell) {
  if (*spell == *Spells::REVEAL) {
    app->hero->currentLocation->reveal();
    app->hero->monsterSense = true;
    app->hero->commit("reveal", 0);
    app->invalidate("reveal");
    app->hero->monsterSense = false;
  } else if (*spell == *Spells::MONSTER_SENSE) {
    app->hero->monsterSense = !app->hero->monsterSense;
    // } else if (*spell == *Spells::MONSTER_FREEZE) {
    //   for (auto o : app->hero->currentLocation->objects) {
    //     if (auto e = std::dynamic_pointer_cast<Enemy>(o)) {
    //       e->type.aiType = AIType::NO_AI;
    //     }
    //   }
    //   app->statusLine->setContent({F("Enemy freezed!")});
  } else if (*spell == *Spells::SUMMON_ORK) {
    auto c = app->hero->currentLocation->cells[app->hero->currentCell->y + 1]
                                              [app->hero->currentCell->x];
    app->hero->currentLocation->objects.push_back(
        mkEnemy(app->hero->currentLocation, c, app->hero, EnemyType::ORK));
    app->hero->commit("summon ork", 0);
  } else if (*spell == *Spells::IDENTIFY) {
    for (auto i : app->hero->inventory) {
      app->hero->identify(i);
    }

    MessageEvent me(nullptr, "Your inventory was identified");
    eb::EventBus::FireEvent(me);
  } else if (*spell == *Spells::SUMMON_PLATE) {
    auto c = app->hero->currentLocation->cells[app->hero->currentCell->y + 1]
                                              [app->hero->currentCell->x];
    // auto item = Prototype::GOD_PLATE->roll();
    auto lt = LootBox{1, {Prototype::POTION_MANA}};
    auto items = lt.open();
    items.front()->currentCell = c;
    app->hero->currentLocation->objects.push_back(items.front());
    app->hero->commit("summon plate", 0);
  } else if (*spell == *Spells::HEAL_LESSER) {
    auto heal = R::Z(app->hero->HP_MAX(app->hero.get()) / 100 * 10,
                     app->hero->HP_MAX(app->hero.get()) / 100 * 25);
    app->hero->hp += heal;
    if (app->hero->HP(app->hero.get()) > app->hero->HP_MAX(app->hero.get())) {
      app->hero->hp = app->hero->HP_MAX(app->hero.get());
    }
    app->hero->commit("heal", 0);
    MessageEvent me(nullptr, fmt::format("You healed {} hp", heal));
    eb::EventBus::FireEvent(me);
  } else if (*spell == *Spells::HEAL) {
    auto heal = R::Z(app->hero->HP_MAX(app->hero.get()) / 100 * 25,
                     app->hero->HP_MAX(app->hero.get()) / 100 * 50);
    app->hero->hp += heal;
    if (app->hero->HP(app->hero.get()) > app->hero->HP_MAX(app->hero.get())) {
      app->hero->hp = app->hero->HP_MAX(app->hero.get());
    }
    app->hero->commit("heal", 0);
    MessageEvent me(nullptr, fmt::format("You healed {} hp", heal));
  } else if (*spell == *Spells::HEAL_GREATER) {
    auto heal = R::Z(app->hero->HP_MAX(app->hero.get()) / 100 * 50,
                     app->hero->HP_MAX(app->hero.get()) / 100 * 100);
    app->hero->hp += heal;
    if (app->hero->HP(app->hero.get()) > app->hero->HP_MAX(app->hero.get())) {
      app->hero->hp = app->hero->HP_MAX(app->hero.get());
    }
    app->hero->commit("heal", 0);
    MessageEvent me(nullptr, fmt::format("You healed {} hp", heal));
  } else if (*spell == *Spells::RESTORE_MANA) {
    auto heal = R::Z(app->hero->MP_MAX(app->hero.get()) / 100 * 25,
                     app->hero->MP_MAX(app->hero.get()) / 100 * 50);
    app->hero->mp += heal;
    if (app->hero->MP(app->hero.get()) > app->hero->MP_MAX(app->hero.get())) {
      app->hero->mp = app->hero->MP_MAX(app->hero.get());
    }
    app->hero->commit("mana", 0);
    MessageEvent me(nullptr, fmt::format("Your {} mp restored", heal));
    eb::EventBus::FireEvent(me);
  } else if (*spell == *Spells::TELEPORT_RANDOM) {
    auto room = app->hero->currentLocation
                    ->rooms[rand() % app->hero->currentLocation->rooms.size()];
    auto cell = room->cells[rand() % room->cells.size()];
    app->hero->currentCell = cell;
    app->hero->commit("Teleport", 0);
    MessageEvent me(nullptr, fmt::format("You were teleported."));
    eb::EventBus::FireEvent(me);
  } else if (auto tspell = std::dynamic_pointer_cast<ToggleTraitSpell>(spell)) {
    if (R::R() < tspell->probability) {
      if (app->hero->hasTrait(tspell->trait)) {
        app->hero->traits.erase(std::remove(
            app->hero->traits.begin(), app->hero->traits.end(), tspell->trait));
        MessageEvent me(nullptr, fmt::format("Undo {}", tspell->name));
        eb::EventBus::FireEvent(me);
      } else {
        app->hero->traits.push_back(tspell->trait);
        MessageEvent me(nullptr, fmt::format("Apply {}", tspell->name));
        eb::EventBus::FireEvent(me);
      }
    } else {
      MessageEvent me(nullptr, fmt::format("Nothing happens"));
      eb::EventBus::FireEvent(me);
    }
    app->hero->commit("toggle trait", 0);
  } else if (auto espell = std::dynamic_pointer_cast<EffectSpell>(spell)) {

    MessageEvent me(nullptr, fmt::format("Apply {} effect", espell->name));
    eb::EventBus::FireEvent(me);
    app->hero->activeEffects.push_back(espell->effect);
    app->hero->commit("apply effect", 0);
  }
}

void EventReactor::onEvent(HeroDiedEvent &e) {
  app->modeManager.toGameOver();
  app->gameOverMode->render(app->gameOverState);
}
