#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <utility>

#include "cinder/Rand.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderPango.h"
#include "lss/modes.hpp"
#include "lss/state.hpp"

#include "lss/game/player.hpp"
#include "lss/game/enemy.hpp"
#include "lss/game/door.hpp"
#include "lss/game/events.hpp"

#include "lss/ui/statusLine.hpp"
#include "rang.hpp"
#include "format.h"

#include "EventHandler.hpp"
#include "EventBus.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

int HOffset = 24;
int VOffset = 24;
// std::string DEFAULT_FONT = "Iosevka 14";
std::string DEFAULT_FONT = "FiraCode 12";
auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

class LSSApp : public App
             , public eb::EventHandler<DoorOpenedEvent>
             , public eb::EventHandler<EnemyTakeDamageEvent>
             , public eb::EventHandler<EnemyDiedEvent>
{
public:
  void setup() override;
  void mouseDown(MouseEvent event) override;
  void update() override;
  void keyDown(KeyEvent event) override;
  void draw() override;
  void invalidate();

  kp::pango::CinderPangoRef mPango;
  kp::pango::CinderPangoRef statusFrame;
  ModeManager modeManager = ModeManager();
  std::shared_ptr<HintsMode> hints;
  std::shared_ptr<StatusLine> statusLine;
  std::shared_ptr<State> state;
  std::shared_ptr<Player> hero;

    virtual void onEvent(DoorOpenedEvent & e) override;
    virtual void onEvent(EnemyTakeDamageEvent & e) override;
    virtual void onEvent(EnemyDiedEvent & e) override;
};


void LSSApp::onEvent(DoorOpenedEvent & e) {
    statusLine->setContent({std::make_shared<Fragment>("Door opened")});
}
void LSSApp::onEvent(EnemyTakeDamageEvent & e) {
    statusLine->setContent({std::make_shared<Fragment>(fmt::format("You hit enemy: {} dmg", e.damage))});
}
void LSSApp::onEvent(EnemyDiedEvent & e) {
    invalidate();
    statusLine->setContent({std::make_shared<Fragment>("Enemy died")});
}

void LSSApp::setup() {
  kp::pango::CinderPango::setTextRenderer(kp::pango::TextRenderer::FREETYPE);
  mPango = kp::pango::CinderPango::create();
  mPango->setMinSize(800, 600);
  mPango->setMaxSize(getWindowWidth(), getWindowHeight());
  mPango->setSpacing(mPango->getSpacing() - 4.0);

  statusFrame = kp::pango::CinderPango::create();
  statusFrame->setMinSize(getWindowWidth(), StatusLine::HEIGHT);
  statusFrame->setMaxSize(getWindowWidth(), StatusLine::HEIGHT);

  state = std::make_shared<State>();

  statusLine = std::make_shared<StatusLine>(state);
  statusLine->setContent(State::normal_mode);

  hero = std::make_shared<Player>();
  // hero->currentCell = std::make_shared<Cell>(CellType::FLOOR);
  // hero->currentCell->visibilityState = VisibilityState::VISIBLE;
  hero->currentLocation = std::make_shared<Location>();
  // hero->currentLocation->cells.push_back({})

  std::string line;
  std::ifstream dungeon_file("./dungeon.ascii");
  int n, i;
  if (dungeon_file.is_open()) {
    n = 0;
    while (getline(dungeon_file, line)) {
      hero->currentLocation->cells.push_back({});
      i = 0;
      for (auto ch : line) {
        auto c = std::make_shared<Cell>(i, n, CellType::WALL);
        c->visibilityState = VisibilityState::UNKNOWN;
        c->type = CellType::UNKNOWN_CELL;

        switch (ch) {
        case '.':
          c->type = CellType::FLOOR;
          break;
        case '#':
          c->type = CellType::WALL;
          c->passThrough = false;
          c->seeThrough = false;
          break;
        case '+':
        {
          auto door = std::make_shared<Door>();
          door->currentCell = c;
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(door);
        }
        break;
        case 'e':
          auto enemy = std::make_shared<Enemy>();
          enemy->currentCell = c;
          c->type = CellType::FLOOR;
          c->passThrough = true;
          hero->currentLocation->objects.push_back(enemy);
          break;
        }
        hero->currentLocation->cells[n].push_back(c);
        i++;
      }
      n++;
    }
    dungeon_file.close();
  }
  state->currentPalette = palettes::DARK;
  hero->currentCell = hero->currentLocation->cells[15][30];
  hero->calcViewField();

  state->fragments.assign(n * (i+1), std::make_shared<Unknown>());

  invalidate();

  eb::EventBus::AddHandler<DoorOpenedEvent>(*this);
  eb::EventBus::AddHandler<EnemyDiedEvent>(*this);
  eb::EventBus::AddHandler<EnemyTakeDamageEvent>(*this);

  eb::EventBus::AddHandler<EnemyDiedEvent>(*hero->currentLocation);
}

void LSSApp::invalidate() {
  auto t0 = std::chrono::system_clock::now();
  auto row = 0;
  auto index = 0;
  for (auto r : hero->currentLocation->cells) {
    auto column = 0;
    for (auto c : r) {
      auto cc = hero->currentCell;
      if (hero->canSee(c)) {
        c->visibilityState = VisibilityState::VISIBLE;
      } else if (c->visibilityState == VisibilityState::VISIBLE) {
        c->visibilityState = VisibilityState::SEEN;
      }

      auto f = state->fragments[index];
      if (c->visibilityState == VisibilityState::UNKNOWN) {
        if (!std::dynamic_pointer_cast<Unknown>(f)) {
          state->fragments[index] = std::make_shared<Unknown>();
        }
      } else if (c->visibilityState == VisibilityState::SEEN) {
        switch (c->type) {
        case CellType::UNKNOWN_CELL:
          if (!std::dynamic_pointer_cast<Unknown>(f)) {
            state->fragments[index] = std::make_shared<Unknown>();
          }
          break;
        case CellType::WALL:
          if (!std::dynamic_pointer_cast<WallSeen>(f)) {
            state->fragments[index] = std::make_shared<WallSeen>();
          }
          break;
        case CellType::FLOOR:
          if (!std::dynamic_pointer_cast<FloorSeen>(f)) {
            state->fragments[index] = std::make_shared<FloorSeen>();
          }
          break;
        }
      } else {
        switch (c->type) {
        case CellType::UNKNOWN_CELL:
          if (!std::dynamic_pointer_cast<Unknown>(f)) {
            state->fragments[index] = std::make_shared<Unknown>();
          }
          break;
        case CellType::WALL:
          if (!std::dynamic_pointer_cast<Wall>(f)) {
            state->fragments[index] = std::make_shared<Wall>();
          }
          break;
        case CellType::FLOOR:
          if (!std::dynamic_pointer_cast<Floor>(f)) {
            state->fragments[index] = std::make_shared<Floor>();
          }
          break;
        }
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
    if (!hero->canSee(ec)) continue;
    auto index = ec->y * (hero->currentLocation->cells.front().size() + 1) + ec->x;

    if (auto e = std::dynamic_pointer_cast<Enemy>(o)) {
        state->fragments[index] = std::make_shared<EnemySign>();
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

  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  // if (ms.count() > 5) {
  //   std::cout << "invalidate time taken: " << rang::fg::green << ms.count()
  //             << rang::style::reset << '\n';
  // }
}

void LSSApp::mouseDown(MouseEvent event) {}

void LSSApp::keyDown(KeyEvent event) {

  // if (modeManager.modeFlags->isHints) {
  //     auto handled = hints->processKey(event);
  //     if (handled) {
  //         if (!hints->activated) {
  //             modeManager.toNormal();
  //             statusLine->setContent(State::normal_mode);
  //         }
  //         return;
  //     }
  // }

  modeManager.processKey(event);

  if (modeManager.modeFlags->isNormal) {
    statusLine->setContent(State::normal_mode);
  } else if (modeManager.modeFlags->isHints) {
    hints->activated = true;
    statusLine->setContent(State::hints_mode);
  } else if (modeManager.modeFlags->isLeader) {
    statusLine->setContent(State::leader_mode);
  }

  switch (event.getCode()) {
  case KeyEvent::KEY_k:
    if (hero->move(Direction::N)) invalidate();
    break;
  case KeyEvent::KEY_l:
    if (hero->move(Direction::E)) invalidate();
    invalidate();
    break;
  case KeyEvent::KEY_j:
    if (hero->move(Direction::S)) invalidate();
    invalidate();
    break;
  case KeyEvent::KEY_h:
    if (hero->move(Direction::W)) invalidate();
    invalidate();
    break;
  case KeyEvent::KEY_q:
    exit(0);
    break;
  case KeyEvent::KEY_r:
    // State::greeting[0]->template_str = "!!!";
    break;
  case KeyEvent::KEY_t:
    state->currentPalette = state->currentPalette.name == palettes::DARK.name
                                ? palettes::LIGHT
                                : palettes::DARK;
    state->invalidate();
    break;
  default:
    break;
  }
}

void LSSApp::update() {
  gl::clear(state->currentPalette.bgColor);
  gl::enableAlphaBlendingPremult();

  if (mPango != nullptr) {

    auto t0 = std::chrono::system_clock::now();
    state->render(mPango);
    auto t1 = std::chrono::system_clock::now();
    using milliseconds = std::chrono::duration<double, std::milli>;
    milliseconds ms = t1 - t0;
    if (ms.count() > 5) {
      // std::cout << "render time taken: " << rang::fg::green << ms.count()
                // << rang::style::reset << '\n';
    }
  }

  // TODO: hide into statusBar. Implement padding
  if (statusFrame != nullptr) {

    statusFrame->setText(state->renderStatus());

    statusFrame->setDefaultTextFont(DEFAULT_FONT);
    statusFrame->render();
  }
}

void LSSApp::draw() {
  if (mPango != nullptr) {

    mPango->setDefaultTextColor(state->currentPalette.fgColor);
    mPango->setBackgroundColor(ColorA(0, 0, 0, 0));
    // gl::color(ColorA(0,0,0,0));
    gl::draw(mPango->getTexture(), vec2(HOffset, VOffset));
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
}

CINDER_APP(LSSApp, RendererGl)
