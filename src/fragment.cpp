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
  tpl.setValue("orange", state->currentPalette.orange);

  tpl.setValue("floor_color", state->currentPalette.floor_color);
  tpl.setValue("wall_color", state->currentPalette.wall_color);
  tpl.setValue("floor_color_seen", state->currentPalette.floor_color_seen);
  tpl.setValue("wall_color_seen", state->currentPalette.wall_color_seen);
  tpl.setValue("hero_color", state->currentPalette.hero_color);

  for (auto[key, value] : args) {
    std::visit([&](auto const &val) { tpl.setValue(key, val); }, value);
  }

  cache = "<span>" + tpl.render() + "</span>";
  damaged = false;
  return cache;
}

std::map<ItemSpec, std::string> itemSigns = {
    {ItemType::CORPSE, "%"},    {ItemType::ROCK, "*"},
    {ItemType::PICK_AXE, "("},  {ItemType::SWORD, "("},
    {ItemType::GOLD_RING, "="}, {ItemType::GOLD_COINS, "$"},
    {ItemType::DAGGER, "("},    {ItemType::TORCH, "]"}, {ItemType::TORCH_STAND, "*"},
};

std::map<ItemSpec, std::string> itemColors = {
    {ItemType::CORPSE, "red"},     {ItemType::ROCK, "gray"},
    {ItemType::PICK_AXE, "white"}, {ItemType::SWORD, "#F7CA88"},
    {ItemType::GOLD_RING, "gold"}, {ItemType::GOLD_COINS, "gold"},
    {ItemType::DAGGER, "gray"},    {ItemType::TORCH, "orange"}, {ItemType::TORCH_STAND, "orange"},
};

std::map<EnemySpec, std::string> enemySigns = {
    {EnemyType::GOBLIN, "g"}, {EnemyType::ORK, "o"}, {EnemyType::PIXI, "p"},
};

std::map<EnemySpec, std::string> enemyColors = {
    {EnemyType::GOBLIN, "green"},
    {EnemyType::ORK, "#22cc22"},
    {EnemyType::PIXI, "pink"},
};

std::map<CellType, std::string> cellSigns = {
    {CellType::FLOOR, "⋅"s},         {CellType::FLOOR_BLOOD, "⋅"s},
    {CellType::WALL, "#"s},          {CellType::UNKNOWN_CELL, " "s},
    {CellType::DOWNSTAIRS, "&gt;"s}, {CellType::UPSTAIRS, "&lt;"s},
};

std::map<CellType, std::map<bool, std::string>> cellColors = {
    {CellType::FLOOR, {{false, "#555"}, {true, "#2f2f2f"}}},
    {CellType::FLOOR_BLOOD, {{false, "darkred"}, {true, "#2f2f2f"}}},
    {CellType::WALL, {{false, "#aaa"}, {true, "#555"}}},
    {CellType::UNKNOWN_CELL, {{false, "#555"}, {true, "#777"}}},
    {CellType::DOWNSTAIRS, {{false, "#aaa"}, {true, "#666"}}},
    {CellType::UPSTAIRS, {{false, "#aaa"}, {true, "#666"}}},
};

std::map<CellType, std::map<bool, std::string>> cellWeights = {
    {CellType::FLOOR, {{false, "normal"}, {true, "normal"}}},
    {CellType::FLOOR_BLOOD, {{false, "normal"}, {true, "normal"}}},
    {CellType::WALL, {{false, "bold"}, {true, "bold"}}},
    {CellType::UNKNOWN_CELL, {{false, "normal"}, {true, "normal"}}},
    {CellType::DOWNSTAIRS, {{false, "normal"}, {true, "normal"}}},
    {CellType::UPSTAIRS, {{false, "normal"}, {true, "normal"}}},
};

std::map<std::string, tpl_arg> getCellArgs(CellType type, bool seen) {
  return {
      {"sign", cellSigns[type]},
      {"color", cellColors[type][seen]},
      {"weight", cellWeights[type][seen]},
  };
}

CellSign::CellSign(CellType type, bool seen)
    : Fragment("<span color='{{color}}' weight='{{weight}}'>{{sign}}</span>",
               getCellArgs(type, seen)) {}
void CellSign::update(CellType type, bool seen) {
  auto new_args = getCellArgs(type, seen);
  if (std::get<std::string>(args["color"]) !=
          std::get<std::string>(new_args["color"]) ||
      std::get<std::string>(args["sign"]) !=
          std::get<std::string>(new_args["sign"])) {
    args = new_args;
    damaged = true;
  }
}
HeroSign::HeroSign()
    : Fragment("<span color='{{hero_color}}' weight='bold'>@</span>") {}
EnemySign::EnemySign(EnemySpec type)
    : Fragment("<span color='{{color}}' weight='bold'>{{sign}}</span>",
               {{"sign", enemySigns[type]}, {"color", enemyColors[type]}}) {}
DoorSign::DoorSign(bool opened)
    : Fragment("<span weight='bold'>{{sign}}</span>",
               {{"sign", opened ? "/"s : "+"s}}) {}
ItemSign::ItemSign(ItemSpec type)
    : Fragment("<span color='{{color}}'>{{sign}}</span>",
               {{"sign", itemSigns[type]}, {"color", itemColors[type]}}) {}
