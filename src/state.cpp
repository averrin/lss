#include "lss/state.hpp"
#include "Jinja2CppLight.h"

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
        std::string DEFAULT_FONT = "FiraCode 12";
        surface->setDefaultTextFont(DEFAULT_FONT);

        surface->setText(cache);
        surface->render();
        return;
    }

    std::string content;
    auto n = 0;
    for (auto f : fragments) {
        content += f->render(*this);
        // surface->setText(content);
        // surface->render();
        // f->index = n;
        // auto c = surface->getCount();
        // f->length = c - n + 1;
        // f->rect = surface->getRect(f->index, f->length);
        // n = c + 2;
    }
    cache = "<tt>" + content + "</tt>";

    damaged = false;
    render(surface);
};

void State::invalidate() {
    damaged = true;
}

std::string State::renderStatus() {
    std::string content;
    for (auto f : statusFragments) {
        content += f->render(*this);
    }
    // std::cout << content << std::endl;
    return content;
};

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

const Fragments State::START = {F("<tt>")};
const Fragments State::END = {F("</tt>")};
const Fragments State::END_LINE = {F("<br>")};

const Fragments State::normal_mode = {F("<span>NORMAL</span>" )};
const Fragments State::hints_mode =  {F("<span color='{{green}}'>HINTS</span>" )};
const Fragments State::leader_mode = {F("<span color='{{blue}}'>LEADER</span>" )};
