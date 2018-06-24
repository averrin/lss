#include "lss/eventReactor.hpp"
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

void EventReactor::onEvent(eb::Event &e) { app->invalidate(); }
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
