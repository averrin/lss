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

void EventReactor::onEvent(EnemyTakeDamageEvent &e) {
  if (e.damage->damage == 0)
    return;
  auto enemy = std::dynamic_pointer_cast<Enemy>(e.getSender());

  auto a = std::make_shared<ColorAnimation>(enemy, Color::fromHexString("#ff0000"), 8, true);
  app->animations.push_back(a);
}

void EventReactor::onEvent(HeroTakeDamageEvent &e) {
  if (e.damage->damage == 0)
    return;
  auto a =
      std::make_shared<ColorAnimation>(app->hero, Color::fromHexString("#ff0000"), 8, true);
  app->animations.push_back(a);
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
    app->gameGrid->clear();
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

    app->gameGrid->clear();
    app->hero->currentLocation->updateView(app->hero);
    app->hero->commit("down in er", 0);
  } else {
    MessageEvent me(nullptr, "There is no suitable stair.");
    eb::EventBus::FireEvent(me);
    return;
  }
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
    // auto caster = std::dynamic_pointer_cast<Creature>(e.getSender());
    return app->magic->castSpell(app->hero, spell);
  }
  app->objectSelectMode->setHeader(F("Items to use: "));

  auto usable = utils::castObjects<Consumable>(app->hero->inventory);
  app->objectSelectMode->setObjects(utils::castObjects<Object>(usable));

  Formatter formatter = [](std::shared_ptr<Object> o, std::string letter) {
    if (auto item = std::dynamic_pointer_cast<Consumable>(o)) {
      return fmt::format("<span weight='bold'>{}</span> - {}", letter,
                         item->getFullTitle());
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

void EventReactor::onEvent(PauseEvent &e) {
  app->invalidate();
  app->pauseMode->setCallback(e.callback);
  app->statusLine->setContent(State::pause_mode);
  app->modeManager.toPause();
}

void EventReactor::onEvent(DirectionEvent &e) {
  app->directionMode->setCallback(e.callback);
  app->statusLine->setContent(State::direction_mode);
  app->modeManager.toDirection();
}

void EventReactor::onEvent(TargetEvent &e) {
  app->state->selection.clear();
  app->targetMode->setCallback(e.callback);
  app->targetMode->setCheckTarget(e.checkTarget);
  app->statusLine->setContent(State::target_mode);

  if (e.startTarget == nullptr) {
    app->state->cursor = {app->hero->currentCell->x, app->hero->currentCell->y};
  } else {
    app->state->cursor = {e.startTarget->x, e.startTarget->y};

    auto location = app->hero->currentLocation;
    auto line = location->getLine(app->hero->currentCell, e.startTarget);
    for (auto c : line) {
      app->state->selection.push_back({{c->x, c->y}, "#6c6d79"});
    }
  }
  app->state->setSelect(true);
  app->state->invalidate("target event");
  app->modeManager.toTarget();
}

void EventReactor::onEvent(ZapCommandEvent &e) {
  if (e.spell != nullptr)
    return;

  app->objectSelectMode->setHeader(F("Spells for zap: "));

  if (app->debug) {
    app->objectSelectMode->setObjects(
        utils::castObjects<Object>(Spells::USABLE));
  } else {
    std::vector<std::shared_ptr<Spell>> spells(Spells::USABLE.size());
    auto it = std::copy_if(
        Spells::USABLE.begin(), Spells::USABLE.end(), spells.begin(),
        [&](auto spell) {
          return spell->cost != 0 &&
                 spell->level <=
                     (app->hero->INTELLIGENCE(app->hero.get()) - 1) * 10;
        });

    spells.resize(std::distance(spells.begin(), it));
    std::sort(spells.begin(), spells.end(),
              [](auto s1, auto s2) { return s1->level < s2->level; });

    app->objectSelectMode->setObjects(utils::castObjects<Object>(spells));
  }

  Formatter formatter = [&](std::shared_ptr<Object> o, std::string letter) {
    if (auto spell = std::dynamic_pointer_cast<Spell>(o)) {
      auto castable = spell->cost <= app->hero->MP(app->hero.get());

      std::string manaLine = "        ";
      int m =
          spell->cost / app->hero->MP_MAX(app->hero.get()) * manaLine.size();
      std::fill_n(manaLine.begin(), m, '|');
      return fmt::format(
          "<span{}><span weight='bold'>{}</span> - {:32} <b>MP:[<span "
          "color='{{{{blue}}}}'>{}</span>]</b></span>",
          castable ? "" : " color='gray'", letter, spell->name, manaLine);
    }
    return "Unknown error"s;
  };
  app->objectSelectMode->setFormatter(formatter);

  app->objectSelectMode->setCallback([&](std::shared_ptr<Object> o) {
    auto spell = std::dynamic_pointer_cast<Spell>(o);
    auto castable = spell->cost <= app->hero->MP(app->hero.get());
    if (!castable)
      return false;
    auto e = std::make_shared<ZapCommandEvent>(app->hero, spell);
    eb::EventBus::FireEvent(*e);
    if (app->modeManager.modeFlags->currentMode == Modes::OBJECTSELECT) {
      app->modeManager.toNormal();
    }
    return true;
  });

  app->objectSelectMode->render(app->objectSelectState);
  app->modeManager.toObjectSelect();
  app->statusLine->setContent(State::text_mode);
}

void EventReactor::onEvent(HeroDiedEvent &e) {
  app->modeManager.toGameOver();
  app->gameOverMode->render(app->gameOverState);
}

void EventReactor::onEvent(AnimationEvent &e) {
  app->animations.push_back(e.animation);
}

void EventReactor::onEvent(HeroCommandEvent &e) {
  app->modeManager.toHero();
  app->heroMode->render(app->heroState);
}

// TODO: move to hero
void EventReactor::onEvent(LightCommandEvent &e) {
  auto slot = app->hero->getSlot(WearableType::LIGHT);
  if (slot->item != nullptr) {
    app->hero->unequip(slot);
  } else {
    if (auto torch = std::find_if(
            app->hero->inventory.begin(), app->hero->inventory.end(),
            [](auto i) {
              return i->type.wearableType == WearableType::LIGHT &&
                     i->durability > 0;
            });
        torch != app->hero->inventory.end()) {
      app->hero->equip(slot, *torch);
    }
  }
  app->hero->currentLocation->invalidate("equip light");
  app->hero->viewField = app->hero->calcViewField();
  app->hero->currentLocation->updateLight(app->hero);
  app->hero->currentLocation->updateView(app->hero);
  app->invalidate("equip light");
}

void EventReactor::onEvent(ThrowCommandEvent &e) {
  if (e.item != nullptr)
    return;

  app->objectSelectMode->setHeader(F("Items to throw: "));

  Items dropable(app->hero->inventory.size());
  auto it =
      std::copy_if(app->hero->inventory.begin(), app->hero->inventory.end(),
                   dropable.begin(),
                   [](std::shared_ptr<Item> item) { return !item->equipped; });

  dropable.resize(std::distance(dropable.begin(), it));

  app->objectSelectMode->setObjects(utils::castObjects<Object>(dropable));

  Formatter formatter = [](std::shared_ptr<Object> o, std::string letter) {
    if (auto item = std::dynamic_pointer_cast<Item>(o)) {
      return fmt::format("<span weight='bold'>{}</span> - {}", letter,
                         item->getFullTitle());
    }
    return "Unknown error"s;
  };
  app->objectSelectMode->setFormatter(formatter);

  app->objectSelectMode->setCallback([=](std::shared_ptr<Object> o) {
    auto item = std::dynamic_pointer_cast<Item>(o);
    app->state->selection.clear();
    app->targetMode->setCallback([=](auto cell) {
      ThrowCommandEvent me(item, cell);
      eb::EventBus::FireEvent(me);
      app->modeManager.toNormal();
      app->statusLine->setContent(State::normal_mode);
      return true;
    });
    app->targetMode->setCheckTarget([&](auto line) {
      auto cell = line.back();
      if (!cell->type.passThrough)
        return false;
      if (line.size() > app->hero->getThrowRange())
        return false;
      auto pti = std::find_if(line.begin(), line.end(), [&](auto c) {
        return !c->passThrough && c != line.back() &&
               c != app->hero->currentCell;
      });
      if (pti != line.end())
        return false;
      return true;
    });
    app->modeManager.toTarget();
    app->statusLine->setContent(State::target_mode);
    auto n = app->hero->currentLocation->getNeighbors(app->hero->currentCell);
    auto s = std::find_if(n.begin(), n.end(),
                          [](auto c) { return c->type.passThrough; });
    app->state->cursor = {(*s)->x, (*s)->y};
    app->state->setSelect(true);
    app->state->invalidate("throw event");
    return true;
  });

  app->objectSelectMode->render(app->objectSelectState);
  app->modeManager.toObjectSelect();
}
