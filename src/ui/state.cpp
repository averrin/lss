#include <chrono>
#include <rang.hpp>

#include "Jinja2CppLight.h"
#include "ui/state.hpp"

using namespace Jinja2CppLight;
using namespace std::string_literals;

void State::setSelect(bool val) {
  auto needInvalidate = val != select;
  select = val;
  if (needInvalidate)
    invalidate();
}

void State::setContent(Fragments content) {
  fragments = content;
  damaged = true;
};

void State::clear() { setContent({}); }

void State::appendContent(Fragments content) {
  fragments.insert(fragments.end(), content.begin(), content.end());
  damaged = true;
};

void State::appendContent(std::shared_ptr<Fragment> content) {
  fragments.push_back(content);
  damaged = true;
};

void State::setFragment(int i, std::shared_ptr<Fragment> f) {
  fragments[i] = f;
}

std::string State::renderFragment(std::shared_ptr<Fragment> f) {
  std::string fContent;
  auto n = std::distance(fragments.begin(),
                         std::find(fragments.begin(), fragments.end(), f));
  auto isCursor = select && n == cursor.y * (width + 1) + cursor.x;
  auto selectionIt =
      std::find_if(selection.begin(), selection.end(), [&](Selection s) {
        return s.pos.y * (width + 1) + s.pos.x == n;
      });
  auto hasBg = isCursor || (select && selectionIt != selection.end());

  if (hasBg) {
    auto color = COLORS::CURSOR;
    if (!isCursor && selectionIt != selection.end()) {
      color = (*selectionIt).color;
    }
    f->invalidate(true);
    fContent = f->render(this, {{"bgColor", color}, {"bgAlpha", 100}});
    // std::cout << fContent << " override bgColor: " << color <<std::endl;
  } else {
    fContent = f->render(this);
    // std::cout << fContent <<std::endl;
  }
  return fContent;
}

std::string State::render() {
  std::string content;
  for (auto f : fragments) {
    content.append(renderFragment(f));
  }
  cache = fmt::format("<tt>{}</tt>", content);
  return cache;
}

void State::render(std::shared_ptr<TextGrid> grid) {
  if (!damaged) {
    auto x = 0;
    auto y = 0;
    auto n = 0;
    for (auto f : fragments) {
      auto selectionIt =
          std::find_if(selection.begin(), selection.end(), [&](Selection s) {
            return s.pos.y * (width + 1) + s.pos.x == n;
          });
      auto isCursor = (x == cursor.x && y == cursor.y) || selectionIt != selection.end();
      if (f->damaged || isCursor) {
        grid->setFragment(x, y, renderFragment(f));
      }
      x++;
      n++;
      if (fragments[n] == END_LINE.front()) {
        x = 0;
        y++;
      }
    }
    grid->render();
    return;
  }

  damaged = false;
  render(grid);
}

void State::render(pango::SurfaceRef surface) {
  if (!damaged) {
    surface->setDefaultTextColor(currentPalette.fgColor);

    surface->setText(cache);
    // auto t0 = std::chrono::system_clock::now();
    surface->render();
    // auto t1 = std::chrono::system_clock::now();
    // using milliseconds = std::chrono::duration<double, std::milli>;
    // milliseconds ms = t1 - t0;
    // std::cout << "settext + render time taken: " << rang::fg::green
    // << ms.count() << rang::style::reset << '\n';
    return;
  }
  render();

  damaged = false;
  render(surface);
};

void State::invalidate(std::string reason, int index) {
  fragments.at(index)->invalidate();
  invalidate(reason);
}

void State::invalidateSelection(std::string reason) {
  auto ci = cursor.y * (width + 1) + cursor.x;
  fragments.at(ci)->invalidate();
  for (auto s : selection) {
    auto si = s.pos.y * (width + 1) + s.pos.x;
    fragments.at(si)->invalidate();
  }
  invalidate(reason);
}

void State::invalidate(std::string reason) {
  log.info("STATE",
           fmt::format("state invalidate [{}]", lu::magenta(reason)));
  invalidate();
}
void State::invalidate() { damaged = true; }

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

const Fragments State::START = {F("<tt>")};
const Fragments State::END = {F("</tt>")};
const Fragments State::END_LINE = {F("<br>")};

const Fragments State::normal_mode = {F("")};
const Fragments State::hints_mode = {F("<span color='{{green}}'>HINTS</span>")};
const Fragments State::direction_mode = {
    F("<span color='{{green}}'>Choose direction: <span "
      "weight='bold'>[yuhjklbn]</span></span>")};
const Fragments State::insert_mode = {
    F("<span color='{{red}}' weight='bold'>/</span>")};
const Fragments State::leader_mode = {
    F("<span color='{{blue}}'>LEADER</span>")};
const Fragments State::unknown_command = {
    F("<span color='{{red}}' weight='bold'>Unknown command</span>")};
const Fragments State::object_select_mode = {
    F("<span color='{{blue}}'>Select item</span> [a-y] and [&lt;&gt;] for "
      "changing page or [Zz, Esc] for exit")};
const Fragments State::text_mode = {F("[Zz, Esc] for exit")};
const Fragments State::pause_mode = {F("Paused. Press any key for continue")};
const Fragments State::inspect_mode = {
    F("<span color='{{blue}}'>Select cell</span> [<b>yuhjklbn</b>] or [Zz, "
      "Esc] for exit")};
const Fragments State::target_mode = {
    F("<span color='{{blue}}'>Select cell</span> [<b>yuhjklbn</b>] or [Zz, "
      "Esc] for exit")};

const Fragments State::HELP_HEADER = {F("<span weight='bold'>Help</span>")};
const Fragments State::INVENTORY_HEADER = {
    F("<span weight='bold'>Inventory</span>")};

const Fragments State::GAMEOVER = {
    F("<span weight='bold' color='red' size='large'>GAME OVER</span>")};

const Fragments State::HELP = {F(
    "<span weight='bold'>Controls</span><br>"
    "• <span color='{{orange}}' weight='bold'>/</span> — insert mode<br>"
    "• <span color='{{orange}}' weight='bold'>q</span> — quit [`/quit`, "
    "`/q`]<br>"
    "• <span color='{{orange}}' weight='bold'>?</span> — help [`/help`, "
    "`/h`]<br>"
    "• <span color='{{orange}}' weight='bold'>yuhjklbn</span> — move [`/m`, "
    "`/move`, `/n`, `/e`, `/s`, `/w`, `/nw`, `/ne`, "
    "`/se`, `/sw`]<br>"
    "```<br>"
    "      y k u<br>"
    "       \\|/ <br>"
    "      h-•-l<br>"
    "       /|\\ <br>"
    "      b j n<br>"
    "```<br>"
    "• <span color='{{orange}}' weight='bold'>d</span>[yuhjklbn] — dig "
    "[`/dig`, `/d`]<br>"
    "• <span color='{{orange}}' weight='bold'>a</span>[yuhjklbn] — attack "
    "[`/attack`, `/a`]<br>"
    "• <span color='{{orange}}' weight='bold'>w</span>[yuhjklbn] — walk "
    "until obstacle [`/walk`]<br>"
    "• <span color='{{orange}}' weight='bold'>p</span> or <span "
    "color='{{orange}}' weight='bold'>,</span> — pick [`/pick`, "
    "`/p`]<br>"
    "• <span color='{{orange}}' weight='bold'>e</span> — equip [`/equip`, "
    "`/eq`]<br>"
    "• <span color='{{orange}}' weight='bold'>shift+d</span> — drop [`/drop`, "
    "`/dr`]<br>"
    "• <span color='{{orange}}' weight='bold'>shift+u</span> — use [`/use`, "
    "`/u`]<br>"
    "• <span color='{{orange}}' weight='bold'>shift+l</span> — [un]equip "
    "[`/light`, "
    "`/l`]<br>"
    "• <span color='{{orange}}' weight='bold'>t</span> — throw item "
    "[`/throw`, "
    "`/t`]<br>"
    "• <span color='{{orange}}' weight='bold'>@</span> — status page "
    "[`/hero`]<br>"
    "• <span color='{{orange}}' weight='bold'>z</span> — cast spell "
    "[`/zap`]<br>"
    "• <span color='{{orange}}' weight='bold'>i</span> — inventory "
    "[`/inventory`, `/i`]<br>"
    "• <span color='{{orange}}' weight='bold'>&lt;</span> — go up "
    "[`/up`]<br>"
    "• <span color='{{orange}}' weight='bold'>&gt;</span> — go down "
    "[`/down`]<br>")};
