#include <variant>

#include "lss/fragment.hpp"
#include "lss/state.hpp"
#include "Jinja2CppLight.h"

using namespace Jinja2CppLight;
using namespace std::string_literals;

Fragment::Fragment(std::string t, std::map<std::string, tpl_arg> a): template_str(t), args(a) {}; 
Fragment::Fragment(std::string t): template_str(t) {}; 
Fragment::Fragment(std::string t, bool n): template_str(t), needRender(n) {}; 

Fragment::~Fragment(){};

std::string Fragment::render(State* state) {
    if (!needRender) {
        return template_str;
    }
    if (!damaged) {
        return cache;
    }

    Template tpl( template_str );

    tpl.setValue( "green", state->currentPalette.green);
    tpl.setValue( "red", state->currentPalette.red);
    tpl.setValue( "blue", state->currentPalette.blue);

    tpl.setValue( "floor_color", state->currentPalette.floor_color);
    tpl.setValue( "wall_color", state->currentPalette.wall_color);
    tpl.setValue( "floor_color_seen", state->currentPalette.floor_color_seen);
    tpl.setValue( "wall_color_seen", state->currentPalette.wall_color_seen);
    tpl.setValue( "hero_color", state->currentPalette.hero_color);

    for (auto [key, value] : args) {
        std::visit([&](auto const & val) { tpl.setValue(key, val); }, value);
    }

    cache = tpl.render();
    damaged = false;
    return cache;
}

Floor::Floor() : Fragment("<span color='{{floor_color}}'>⋅</span>"){}
Wall::Wall() : Fragment("<span color='{{wall_color}}' weight='bold'>#</span>"){}
Hero::Hero() : Fragment("<span color='{{hero_color}}' weight='bold'>@</span>"){}
FloorSeen::FloorSeen() : Fragment("<span color='{{floor_color_seen}}'>⋅</span>"){}
WallSeen::WallSeen() : Fragment("<span color='{{wall_color_seen}}' weight='bold'>#</span>"){}

Unknown::Unknown() : Fragment(" ", false){}
