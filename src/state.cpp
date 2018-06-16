#include "rang.hpp"
#include <chrono>

#include "Jinja2CppLight.h"
#include "lss/state.hpp"

using namespace Jinja2CppLight;
using namespace std::string_literals;

void State::setContent(Fragments content) {
  fragments = content;
  damaged = true;
};

void State::appendContent(Fragments content) {
  fragments.insert(fragments.end(), content.begin(), content.end());
  damaged = true;
};

void State::appendContent(std::shared_ptr<Fragment> content) {
  fragments.push_back(content);
  damaged = true;
};

void State::render(kp::pango::CinderPangoRef surface) {
  if (!damaged) {
    auto t0 = std::chrono::system_clock::now();
    std::string DEFAULT_FONT = "FiraCode 12";
    surface->setDefaultTextFont(DEFAULT_FONT);

    surface->setText(cache);
    surface->render();
    auto t1 = std::chrono::system_clock::now();
    using milliseconds = std::chrono::duration<double, std::milli>;
    milliseconds ms = t1 - t0;
    // if (ms.count() > 5) {
    //   std::cout << "settext + render time taken: " << rang::fg::green
    //             << ms.count() << rang::style::reset << '\n';
    // }
    return;
  }

  std::string content;
  auto n = 0;
  auto t0 = std::chrono::system_clock::now();
  for (auto f : fragments) {
    if (!f->damaged) {
      content.append(f->cache);
    } else {
      content.append(f->render(this));
      n++;
    }
  }
  auto t1 = std::chrono::system_clock::now();
  using milliseconds = std::chrono::duration<double, std::milli>;
  milliseconds ms = t1 - t0;
  // if (ms.count() > 15) {
  //   std::cout << "template render time taken: " << rang::fg::green
  //             << ms.count() << rang::style::reset << '\n';
  // }
  cache = "<tt>" + content + "</tt>";

  damaged = false;
  render(surface);
};

void State::invalidate() { damaged = true; }

std::string State::renderStatus() {
  std::string content;
  for (auto f : statusFragments) {
    content += f->render(this);
  }
  // std::cout << content << std::endl;
  return content;
};

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

const Fragments State::START = {F("<tt>")};
const Fragments State::END = {F("</tt>")};
const Fragments State::END_LINE = {F("<br>")};

const Fragments State::normal_mode = {F("<span>NORMAL</span>")};
const Fragments State::hints_mode = {F("<span color='{{green}}'>HINTS</span>")};
const Fragments State::insert_mode = {
    F("<span color='{{red}}' weight='bold'>/</span>")};
const Fragments State::leader_mode = {
    F("<span color='{{blue}}'>LEADER</span>")};
const Fragments State::unknown_command = {
    F("<span color='{{red}}' weight='bold'>Unknown command</span>")};
