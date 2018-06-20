#include <variant>

#include "Jinja2CppLight.h"
#include "lss/fragment.hpp"
#include "lss/state.hpp"

using namespace Jinja2CppLight;
using namespace std::string_literals;

Fragment::Fragment(std::string t, std::map<std::string, tpl_arg> a)
    : template_str(t), args(a){};
Fragment::Fragment(std::string t) : template_str(t){};
Fragment::Fragment(std::string t, bool n) : template_str(t), needRender(n){};

Fragment::~Fragment(){};

std::string Fragment::render(State *state) {
  if (!needRender) {
    return template_str;
  }
  if (!damaged) {
    return cache;
  }

  Template tpl(template_str);

  tpl.setValue("green", state->currentPalette.green);
  tpl.setValue("red", state->currentPalette.red);
  tpl.setValue("blue", state->currentPalette.blue);

  tpl.setValue("floor_color", state->currentPalette.floor_color);
  tpl.setValue("wall_color", state->currentPalette.wall_color);
  tpl.setValue("floor_color_seen", state->currentPalette.floor_color_seen);
  tpl.setValue("wall_color_seen", state->currentPalette.wall_color_seen);
  tpl.setValue("hero_color", state->currentPalette.hero_color);

  for (auto [key, value] : args) {
    std::visit([&](auto const &val) { tpl.setValue(key, val); }, value);
  }

  cache = "<span>" + tpl.render() + "</span>";
  damaged = false;
  return cache;
}

std::map<ItemType, std::string> itemSigns = {
  {CORPSE, "%"},
  {ROCK, "*"},
  {PICK_AXE, "("},
};
std::string getItemSign(ItemType type){
  return itemSigns[type];
}

std::map<EnemySpec, std::string> enemySigns = {
  {EnemyType::GOBLIN, "g"},
  {EnemyType::ORK, "o"},
  {EnemyType::PIXI, "p"},
};

std::map<EnemySpec, std::string> enemyColors = {
  {EnemyType::GOBLIN, "green"},
  {EnemyType::ORK, "#22cc22"},
  {EnemyType::PIXI, "pink"},
};
std::string getEnemySign(EnemySpec type){
  return enemySigns[type];
}

std::string getEnemyColor(EnemySpec type){
  return enemyColors[type];
}

Floor::Floor() : Fragment("<span color='{{floor_color}}'>⋅</span>") {}
Wall::Wall()
    : Fragment("<span color='{{wall_color}}' weight='bold'>#</span>") {}
HeroSign::HeroSign()
    : Fragment("<span color='{{hero_color}}' weight='bold'>@</span>") {}
EnemySign::EnemySign(EnemySpec type)
    : Fragment("<span color='{{color}}' weight='bold'>{{sign}}</span>",
               {{"sign", getEnemySign(type)}, {"color", getEnemyColor(type)}}) {}
FloorSeen::FloorSeen()
    : Fragment("<span color='{{floor_color_seen}}'>⋅</span>") {}
WallSeen::WallSeen()
    : Fragment("<span color='{{wall_color_seen}}' weight='bold'>#</span>") {}
DoorSign::DoorSign(bool opened)
    : Fragment("<span weight='bold'>{{sign}}</span>",
               {{"sign", opened ? "/"s : "+"s}}) {}
ItemSign::ItemSign(ItemType type)
    : Fragment("<span>{{sign}}</span>",
               {{"sign", getItemSign(type)}}) {}

Unknown::Unknown() : Fragment(" ", false) {}
