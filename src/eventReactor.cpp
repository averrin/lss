#include "lss/eventReactor.hpp"
#include "lss/game/cell.hpp"
#include "lss/game/spell.hpp"
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
  if (app->hero->currentCell->type == CellType::UPSTAIRS && e.dir == StairType::UP) {
    app->hero->currentLocation = app->locations["start"];
    app->hero->currentLocation->enter(app->hero);

    app->state->fragments.assign(
        app->hero->currentLocation->cells.size() *
            (app->hero->currentLocation->cells.front().size() + 1),
        std::make_shared<CellSign>(CellType::UNKNOWN_CELL, false));
    app->hero->commit(0);
    app->invalidate();
  } else if (app->hero->currentCell->type == CellType::DOWNSTAIRS && e.dir == StairType::DOWN) {
    app->hero->currentLocation = app->locations["second"];
    app->hero->currentLocation->enter(app->hero);

    app->state->fragments.assign(
        app->hero->currentLocation->cells.size() *
            (app->hero->currentLocation->cells.front().size() + 1),
        std::make_shared<CellSign>(CellType::UNKNOWN_CELL, false));
    app->hero->commit(0);
    app->invalidate();
  } else {
    MessageEvent me(nullptr, "There is no suitable stair.");
    eb::EventBus::FireEvent(me);
    return;
  }
}

void EventReactor::onEvent(eb::Event &e) { app->invalidate(); }

void EventReactor::onEvent(CommitEvent &e) { app->invalidate(); }

void EventReactor::onEvent(QuitCommandEvent &e) { exit(0); }

void EventReactor::onEvent(HelpCommandEvent &e) {
  app->helpMode->setHeader(State::HELP_HEADER.front());
  app->helpMode->render(app->helpState);
  app->modeManager.toHelp();
  app->statusLine->setContent(State::text_mode);
}
void EventReactor::onEvent(InventoryCommandEvent &e) {
  app->inventoryMode->setHeader(State::INVENTORY_HEADER.front());
  app->inventoryMode->setObjects(
      utils::castObjects<Object>(app->hero->inventory));
  app->inventoryMode->render(app->inventoryState);
  app->modeManager.toInventory();
  app->statusLine->setContent(State::text_mode);
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
                     return item->type.equipable &&
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
        return !item->equipped && item->type.equipable &&
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
                       return item->type.equipable &&
                              std::find(slot->acceptTypes.begin(),
                                        slot->acceptTypes.end(),
                                        item->type.wearableType) !=
                                  slot->acceptTypes.end();
                     }) == app->hero->inventory.end()) {
      shortcut = "   ";
      have_items = false;
    }

    return fmt::format("<span color='{}'>{} {:16} :</span> {}",
                       have_items ? "{{orange}}" : "gray", shortcut, slot->name,
                       slot->item == nullptr
                           ? (have_items ? "-" : "<span color='gray'>-</span>")
                           : slot->item->getTitle());
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

  app->objectSelectMode->setObjects(utils::castObjects<Object>(
      std::vector<std::shared_ptr<Spell>>{Spells::REVEAL, Spells::MONSTER_SENSE,
                                          Spells::MONSTER_FREEZE,
                                          Spells::SUMMON_ORK}));

  Formatter formatter = [](std::shared_ptr<Object> o, std::string letter) {
    auto spell = std::dynamic_pointer_cast<Spell>(o);
    return fmt::format("<span weight='bold'>{}</span> - {}", letter,
                       spell->name);
  };
  app->objectSelectMode->setFormatter(formatter);

  app->objectSelectMode->setCallback([&](std::shared_ptr<Object> o) {
    auto spell = std::dynamic_pointer_cast<Spell>(o);
    auto e = std::make_shared<ZapCommandEvent>(spell);
    eb::EventBus::FireEvent(*e);
    app->modeManager.toNormal();
    return true;
  });

  app->objectSelectMode->render(app->objectSelectState);
  app->modeManager.toObjectSelect();
  app->statusLine->setContent(State::text_mode);
}

std::shared_ptr<Enemy> mkEnemy(std::shared_ptr<Location> location,std::shared_ptr<Cell> c,
                               std::shared_ptr<Player> hero, EnemySpec type) {
  auto enemy = std::make_shared<Enemy>(type);
  enemy->currentCell = c;
  enemy->currentLocation = location;
  return enemy;
}

void EventReactor::castSpell(std::shared_ptr<Spell> spell) {
  if (spell == Spells::REVEAL) {
    app->hero->currentLocation->reveal();
    app->hero->monsterSense = true;
    app->invalidate();
    app->hero->monsterSense = false;
  } else if (spell == Spells::MONSTER_SENSE) {
    app->hero->monsterSense = !app->hero->monsterSense;
    app->invalidate();
  } else if (spell == Spells::MONSTER_FREEZE) {
    for (auto o : app->hero->currentLocation->objects) {
      if (auto e = std::dynamic_pointer_cast<Enemy>(o)) {
        e->type.aiType = AIType::NONE;
      }
    }
    app->statusLine->setContent({F("Enemy freezed!")});
  } else if (spell == Spells::SUMMON_ORK) {
    auto c = app->hero->currentLocation->cells[app->hero->currentCell->y + 1]
                                              [app->hero->currentCell->x];
    app->hero->currentLocation->objects.push_back(
        mkEnemy(app->hero->currentLocation, c, app->hero, EnemyType::ORK));
    app->invalidate();
  }
}
