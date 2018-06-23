#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <utility>

#include "lss/LSSApp.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

std::string VERSION = "0.0.5 by Averrin";

int HOffset = 24;
int VOffset = 24;
// std::string DEFAULT_FONT = "Iosevka 14";
std::string DEFAULT_FONT = "FiraCode 12";
auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

template <typename rT, typename iT>
std::vector<std::shared_ptr<rT>>
castObjects(std::vector<std::shared_ptr<iT>> input) {
  std::vector<std::shared_ptr<rT>> result;
  for (auto input_object : input) {
    if (auto casted_object = std::dynamic_pointer_cast<rT>(input_object)) {
      result.push_back(casted_object);
    }
  }
  return result;
}

QuitCommandEvent::QuitCommandEvent() : CommandEvent(nullptr) {}
std::optional<std::shared_ptr<CommandEvent>>
QuitCommand::getEvent(std::string s) {
  return std::make_shared<QuitCommandEvent>();
}

void LSSApp::onEvent(eb::Event &e) { invalidate(); }
void LSSApp::onEvent(QuitCommandEvent &e) { exit(0); }

void LSSApp::onEvent(HelpCommandEvent &e) {
  helpMode->setHeader(State::HELP_HEADER.front());
  helpMode->render(helpState);
  modeManager.toHelp();
  statusLine->setContent(State::text_mode);
}
void LSSApp::onEvent(InventoryCommandEvent &e) {
  inventoryMode->setHeader(State::INVENTORY_HEADER.front());
  inventoryMode->setObjects(castObjects<Object>(hero->inventory));
  inventoryMode->render(inventoryState);
  modeManager.toInventory();
  statusLine->setContent(State::text_mode);
}

template <typename T>
std::string join_s(const T &array, const std::string &delimiter) {
  std::string res;
  for (auto &element : array) {
    if (!res.empty()) {
      res += delimiter;
    }

    res += element;
  }

  return res;
}

bool LSSApp::slotCallback(std::shared_ptr<Object> o) {
  auto slot = std::dynamic_pointer_cast<Slot>(o);
  fmt::print("Selected slot: {}\n", slot->name);

  if (std::find_if(hero->inventory.begin(), hero->inventory.end(),
                   [slot](std::shared_ptr<Item> item) {
                     return item->type.equipable &&
                            std::find(slot->acceptTypes.begin(),
                                      slot->acceptTypes.end(),
                                      item->type.wearableType) !=
                                slot->acceptTypes.end();
                   }) == hero->inventory.end()) {
    return false;
  }

  if (slot->item != nullptr) {
    auto e = std::make_shared<UnEquipCommandEvent>(slot);
    eb::EventBus::FireEvent(*e);
    objectSelectMode->render(objectSelectState);
    return true;
  }

  objectSelectMode->setHeader(F("Items to equip: "));

  Items equipable(hero->inventory.size());
  auto it = std::copy_if(
      hero->inventory.begin(), hero->inventory.end(), equipable.begin(),
      [slot](std::shared_ptr<Item> item) {
        return !item->equipped && item->type.equipable &&
               std::find(slot->acceptTypes.begin(), slot->acceptTypes.end(),
                         item->type.wearableType) != slot->acceptTypes.end();
      });

  equipable.resize(std::distance(equipable.begin(), it));
  objectSelectMode->setObjects(castObjects<Object>(equipable));

  Formatter formatter = [](std::shared_ptr<Object> o, std::string letter) {
    auto item = std::dynamic_pointer_cast<Item>(o);
    return fmt::format("<span weight='bold'>{}</span> - {}", letter,
                       item->getTitle());
  };
  objectSelectMode->setFormatter(formatter);

  objectSelectMode->setCallback(
      [=](std::shared_ptr<Object> o) { return itemCallback(slot, o); });

  objectSelectMode->render(objectSelectState);
  return true;
}

bool LSSApp::itemCallback(std::shared_ptr<Slot> slot,
                          std::shared_ptr<Object> o) {
  auto item = std::dynamic_pointer_cast<Item>(o);

  auto e = std::make_shared<EquipCommandEvent>(slot, item);
  eb::EventBus::FireEvent(*e);

  e = std::make_shared<EquipCommandEvent>();
  eb::EventBus::FireEvent(*e);
  return true;
}

void LSSApp::onEvent(EquipCommandEvent &e) {
  if (e.item != nullptr)
    return;

  objectSelectMode->setHeader(F("Select slot: "));
  objectSelectMode->setObjects(castObjects<Object>(hero->equipment->slots));

  Formatter formatter = [&](std::shared_ptr<Object> o, std::string letter) {
    auto slot = std::dynamic_pointer_cast<Slot>(o);
    bool have_items = true;
    auto shortcut = fmt::format("<span weight='bold'>{}</span> -", letter);
    if (std::find_if(hero->inventory.begin(), hero->inventory.end(),
                     [slot](std::shared_ptr<Item> item) {
                       return item->type.equipable &&
                              std::find(slot->acceptTypes.begin(),
                                        slot->acceptTypes.end(),
                                        item->type.wearableType) !=
                                  slot->acceptTypes.end();
                     }) == hero->inventory.end()) {
      shortcut = "   ";
      have_items = false;
    }

    return fmt::format("<span color='{}'>{} {:16} :</span> {}",
                       have_items ? "{{orange}}" : "gray", shortcut, slot->name,
                       slot->item == nullptr
                           ? (have_items ? "-" : "<span color='gray'>-</span>")
                           : slot->item->getTitle());
  };
  objectSelectMode->setFormatter(formatter);

  objectSelectMode->setCallback(
      [&](std::shared_ptr<Object> o) { return slotCallback(o); });

  objectSelectMode->render(objectSelectState);

  modeManager.toObjectSelect();
  statusLine->setContent(State::object_select_mode);
}

void LSSApp::setup() {
  kp::pango::CinderPango::setTextRenderer(kp::pango::TextRenderer::FREETYPE);
  gameFrame = kp::pango::CinderPango::create();
  gameFrame->setMinSize(800, 600);
  gameFrame->setMaxSize(getWindowWidth(),
                        getWindowHeight() - StatusLine::HEIGHT);
  gameFrame->setSpacing(gameFrame->getSpacing() - 4.0);

  statusFrame = kp::pango::CinderPango::create();
  statusFrame->setMinSize(getWindowWidth(), StatusLine::HEIGHT);
  statusFrame->setMaxSize(getWindowWidth(), StatusLine::HEIGHT);

  objectSelectFrame = kp::pango::CinderPango::create();
  objectSelectFrame->setMinSize(getWindowWidth(),
                                getWindowHeight() - StatusLine::HEIGHT);
  objectSelectFrame->setMaxSize(getWindowWidth(),
                                getWindowHeight() - StatusLine::HEIGHT);

  helpFrame = kp::pango::CinderPango::create();
  helpFrame->setMinSize(getWindowWidth(),
                        getWindowHeight() - StatusLine::HEIGHT);
  helpFrame->setMaxSize(getWindowWidth(),
                        getWindowHeight() - StatusLine::HEIGHT);

  inventoryFrame = kp::pango::CinderPango::create();
  inventoryFrame->setMinSize(getWindowWidth(),
                             getWindowHeight() - StatusLine::HEIGHT);
  inventoryFrame->setMaxSize(getWindowWidth(),
                             getWindowHeight() - StatusLine::HEIGHT);

  state = std::make_shared<State>();
  statusState = std::make_shared<State>();
  objectSelectState = std::make_shared<State>();
  normalMode = std::make_shared<NormalMode>(this);
  directionMode = std::make_shared<DirectionMode>(this);
  insertMode = std::make_shared<InsertMode>(this);
  objectSelectMode = std::make_shared<ObjectSelectMode>(this);
  helpMode = std::make_shared<HelpMode>(this);
  helpState = std::make_shared<State>();
  inventoryMode = std::make_shared<InventoryMode>(this);
  inventoryState = std::make_shared<State>();

  statusLine = std::make_shared<StatusLine>(statusState);
  statusLine->setContent(State::normal_mode);

  loadMap();
  invalidate();

  setListeners();

  commands.push_back(std::make_shared<MoveCommand>());
  commands.push_back(std::make_shared<QuitCommand>());
  commands.push_back(std::make_shared<PickCommand>());
  commands.push_back(std::make_shared<DigCommand>());
  commands.push_back(std::make_shared<WalkCommand>());
  commands.push_back(std::make_shared<AttackCommand>());
  commands.push_back(std::make_shared<EquipCommand>());
  commands.push_back(std::make_shared<HelpCommand>());
  commands.push_back(std::make_shared<InventoryCommand>());
}

std::shared_ptr<Enemy> makeEnemy(std::shared_ptr<Cell> c,
                                 std::shared_ptr<Player> hero, EnemySpec type) {
  auto enemy = std::make_shared<Enemy>(type);
  enemy->currentCell = c;
  enemy->currentLocation = hero->currentLocation;
  enemy->registration = eb::EventBus::AddHandler<CommitEvent>(*enemy, hero);
  return enemy;
}

void LSSApp::loadMap() {
  hero = std::make_shared<Player>();
  hero->currentLocation = std::make_shared<Location>();
  hero->currentLocation->player = hero;

  std::string line;
  std::ifstream dungeon_file("./dungeon.ascii");
  int n = 0, i = 0;
  if (dungeon_file.is_open()) {
    n = 0;
    while (getline(dungeon_file, line)) {
      hero->currentLocation->cells.push_back({});
      i = 0;
      for (auto ch : line) {
        auto c = std::make_shared<Cell>(i, n, CellType::WALL);
        c->visibilityState = VisibilityState::UNKNOWN;
        c->type = CellType::UNKNOWN_CELL;
        c->passThrough = false;
        c->seeThrough = false;

        switch (ch) {
        case '.':
          c->type = CellType::FLOOR;
          c->passThrough = true;
          c->seeThrough = true;
          break;
        case '#':
          c->type = CellType::WALL;
          c->passThrough = false;
          c->seeThrough = false;
          break;
        case '+': {
          auto door = std::make_shared<Door>();
          door->currentCell = c;
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(door);
        } break;
        case 'o': {
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(
              makeEnemy(c, hero, EnemyType::ORK));
        } break;
        case 'g': {
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(
              makeEnemy(c, hero, EnemyType::GOBLIN));
        } break;
        case 'p': {
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(
              makeEnemy(c, hero, EnemyType::PIXI));
        } break;
        }
        hero->currentLocation->cells[n].push_back(c);
        i++;
      }
      n++;
    }
    dungeon_file.close();
  }
  state->currentPalette = palettes::DARK;
  statusState->currentPalette = palettes::DARK;
  objectSelectState->currentPalette = palettes::DARK;
  helpState->currentPalette = palettes::DARK;
  inventoryState->currentPalette = palettes::DARK;
  hero->currentCell = hero->currentLocation->cells[15][30];
  hero->calcViewField();
  hero->currentLocation->updateView(hero);

  auto axe = std::make_shared<Item>(ItemType::PICK_AXE);
  axe->currentCell = hero->currentLocation->cells[16][31];
  hero->currentLocation->objects.push_back(axe);

  auto sword = std::make_shared<Item>(
      ItemType::SWORD, Effects{std::make_shared<MeleeDamage>(1, 6, 1),
                               std::make_shared<HPModifier>(5)});
  sword->currentCell = hero->currentLocation->cells[15][32];
  hero->currentLocation->objects.push_back(sword);

  state->fragments.assign(
      n * (i + 1), std::make_shared<CellSign>(CellType::UNKNOWN_CELL, false));
}

void LSSApp::setListeners() {
  eb::EventBus::AddHandler<EnemyDiedEvent>(*hero->currentLocation);
  eb::EventBus::AddHandler<ItemTakenEvent>(*hero->currentLocation);
  eb::EventBus::AddHandler<EnterCellEvent>(*hero->currentLocation, hero);
  eb::EventBus::AddHandler<DigEvent>(*hero->currentLocation, hero);
  eb::EventBus::AddHandler<ItemTakenEvent>(*statusLine);
  eb::EventBus::AddHandler<ItemsFoundEvent>(*statusLine);
  eb::EventBus::AddHandler<MessageEvent>(*statusLine);

  eb::EventBus::AddHandler<eb::Event>(*this);
  eb::EventBus::AddHandler<QuitCommandEvent>(*this);
  eb::EventBus::AddHandler<PickCommandEvent>(*hero);
  eb::EventBus::AddHandler<DigCommandEvent>(*hero);
  eb::EventBus::AddHandler<AttackCommandEvent>(*hero);
  eb::EventBus::AddHandler<EquipCommandEvent>(*this);
  eb::EventBus::AddHandler<EquipCommandEvent>(*hero);
  eb::EventBus::AddHandler<UnEquipCommandEvent>(*hero);

  eb::EventBus::AddHandler<HelpCommandEvent>(*this);
  eb::EventBus::AddHandler<InventoryCommandEvent>(*this);

  eb::EventBus::AddHandler<DoorOpenedEvent>(*statusLine);
  eb::EventBus::AddHandler<EnemyDiedEvent>(*statusLine);
  eb::EventBus::AddHandler<EnemyTakeDamageEvent>(*statusLine);
}

void LSSApp::invalidate() {
  auto t0 = std::chrono::system_clock::now();
  auto row = 0;
  auto index = 0;
  for (auto r : hero->currentLocation->cells) {
    auto column = 0;
    for (auto c : r) {
      auto cc = hero->currentCell;
      auto f = state->fragments[index];
      switch (c->visibilityState) {
      case VisibilityState::SEEN:
        // if (auto cs =
        // std::dynamic_pointer_cast<CellSign>(state->fragments[index])) {
        //     cs->update(c->type, true);
        // } else {
        state->fragments[index] = std::make_shared<CellSign>(c->type, true);
        // }
        break;
      case VisibilityState::VISIBLE:
        state->fragments[index] = std::make_shared<CellSign>(c->type, false);
        break;
      }
      column++;
      index++;
    }
    auto f = state->fragments[index];
    if (f != State::END_LINE[0]) {
      state->fragments[index] = State::END_LINE[0];
    }
    index++;
    row++;
  }

  for (auto o : hero->currentLocation->objects) {
    auto ec = o->currentCell;
    if (!hero->canSee(ec))
      continue;
    auto index =
        ec->y * (hero->currentLocation->cells.front().size() + 1) + ec->x;

    if (auto e = std::dynamic_pointer_cast<Enemy>(o)) {
      state->fragments[index] = std::make_shared<EnemySign>(e->type);
    } else if (auto d = std::dynamic_pointer_cast<Door>(o)) {
      state->fragments[index] = std::make_shared<DoorSign>(d->opened);
    } else if (auto i = std::dynamic_pointer_cast<Item>(o)) {
      state->fragments[index] = std::make_shared<ItemSign>(i->type);
    }
  }

  auto hc = hero->currentCell;
  index = hc->y * (hero->currentLocation->cells.front().size() + 1) + hc->x;
  if (!std::dynamic_pointer_cast<HeroSign>(state->fragments[index])) {
    state->fragments[index] = std::make_shared<HeroSign>();
  }

  state->invalidate();
}

void LSSApp::mouseDown(MouseEvent event) {}

void LSSApp::keyDown(KeyEvent event) {

  modeManager.processKey(event);
  if (modeManager.modeFlags->currentMode != Modes::INSERT) {
    typedCommand = "";
  }

  switch (modeManager.modeFlags->currentMode) {
  case Modes::NORMAL:
    statusLine->setContent(State::normal_mode);
    normalMode->processKey(event);
    break;
  case Modes::DIRECTION:
    directionMode->processKey(event);
    break;
  case Modes::INSERT:
    insertMode->processKey(event);
    break;
  case Modes::OBJECTSELECT:
    objectSelectMode->processKey(event);
    break;
  }
}

// TODO: to utils
std::vector<std::string> split_command(std::string strToSplit, char delimeter) {
  std::stringstream ss(strToSplit);
  std::string item;
  std::vector<std::string> splittedStrings;
  while (std::getline(ss, item, delimeter)) {
    splittedStrings.push_back(item);
  }
  return splittedStrings;
}

bool LSSApp::processCommand(std::string cmd) {
  auto s = split_command(cmd, ' ').front();
  auto c = std::find_if(commands.begin(), commands.end(),
                        [s](std::shared_ptr<Command> c) {
                          return std::find(c->aliases.begin(), c->aliases.end(),
                                           s) != c->aliases.end();
                        });
  if (c == commands.end()) {
    statusLine->setContent(State::unknown_command);
    return false;
  }
  auto command = *c;
  auto event = command->getEvent(cmd);
  if (event == std::nullopt)
    return false;

  // TODO: do it automagicaly
  if (auto e = dynamic_pointer_cast<MoveCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<QuitCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<PickCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<DigCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<WalkCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<AttackCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<EquipCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<HelpCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  } else if (auto e = dynamic_pointer_cast<InventoryCommandEvent>(*event)) {
    eb::EventBus::FireEvent(*e);
  }
  invalidate();
  return true;
}

void LSSApp::update() {
  gl::clear(state->currentPalette.bgColor);
  gl::enableAlphaBlendingPremult();

  switch (modeManager.modeFlags->currentMode) {
  case Modes::NORMAL:
  case Modes::INSERT:
  case Modes::DIRECTION:
  case Modes::HINTS:
  case Modes::LEADER:
    state->render(gameFrame);
    break;
  case Modes::OBJECTSELECT:
    objectSelectState->render(objectSelectFrame);
    break;
  case Modes::HELP:
    helpState->render(helpFrame);
    break;
  case Modes::INVENTORY:
    inventoryState->render(inventoryFrame);
    break;
  }

  if (statusFrame != nullptr) {
    statusState->render(statusFrame);
  }
}

void LSSApp::draw() {

  gl::color(state->currentPalette.bgColor);
  gl::drawSolidRect(
      Rectf(0, 0, getWindowWidth(), getWindowHeight() - StatusLine::HEIGHT));
  gl::color(ColorA(1, 1, 1, 1));
  objectSelectFrame->setDefaultTextColor(state->currentPalette.fgColor);
  objectSelectFrame->setBackgroundColor(ColorA(0, 0, 0, 0));

  switch (modeManager.modeFlags->currentMode) {
  case Modes::NORMAL:
  case Modes::INSERT:
  case Modes::DIRECTION:
  case Modes::HINTS:
  case Modes::LEADER:
    gameFrame->setDefaultTextColor(state->currentPalette.fgColor);
    gl::draw(gameFrame->getTexture(), vec2(HOffset, VOffset));
    break;
  case Modes::OBJECTSELECT:
    objectSelectFrame->setDefaultTextColor(state->currentPalette.fgColor);
    gl::draw(objectSelectFrame->getTexture(), vec2(HOffset, VOffset));
    break;
  case Modes::HELP:
    helpFrame->setDefaultTextColor(state->currentPalette.fgColor);
    gl::draw(helpFrame->getTexture(), vec2(HOffset, VOffset));
    break;
  case Modes::INVENTORY:
    inventoryFrame->setDefaultTextColor(state->currentPalette.fgColor);
    gl::draw(inventoryFrame->getTexture(), vec2(HOffset, VOffset));
    break;
  }

  if (statusFrame != nullptr) {
    gl::color(state->currentPalette.bgColorAlt);
    gl::drawSolidRect(Rectf(0, getWindowHeight() - StatusLine::HEIGHT,
                            getWindowWidth(), getWindowHeight()));
    gl::color(ColorA(1, 1, 1, 1));
    statusFrame->setDefaultTextColor(Color(state->currentPalette.fgColor));
    statusFrame->setBackgroundColor(ColorA(0, 0, 0, 0));
    gl::draw(statusFrame->getTexture(),
             vec2(6, getWindowHeight() - StatusLine::HEIGHT + 6));
  }

  gl::drawString(VERSION, vec2(getWindowWidth() - 120,
                               getWindowHeight() - StatusLine::HEIGHT + 12));
}

CINDER_APP(LSSApp, RendererGl)
