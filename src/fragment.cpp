#include <variant>

#include "Jinja2CppLight.h"
#include "lss/fragment.hpp"
#include "lss/state.hpp"

using namespace Jinja2CppLight;
using namespace std::string_literals;

Fragment::Fragment(std::string t, std::map<std::string, tpl_arg> a)
    : template_str(t), args(a){};
Fragment::Fragment(std::string t, std::map<std::string, tpl_arg> a, bool n)
    : template_str(t), args(a), needRender(n){};
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

  for (auto [key, value] : args) {
    std::visit([&](auto const &val) { tpl.setValue(key, val); }, value);
  }

  auto content = tpl.render();
  if (content != "" && content != "<br>" && content[0] != '<') {
    content = "<span>" + content + "</span>";
  }
  cache = content;
  damaged = false;
  return cache;
}

std::map<ItemSpec, std::string> itemSigns = {
    {ItemType::CORPSE, "%"},      {ItemType::ROCK, "*"},
    {ItemType::PICK_AXE, "("},    {ItemType::SWORD, "("},
    {ItemType::GOLD_RING, "="},   {ItemType::GOLD_COINS, "$"},
    {ItemType::DAGGER, "("},      {ItemType::TORCH, "]"},
    {ItemType::TORCH_STAND, "*"},
};

std::map<ItemSpec, std::string> itemColors = {
    {ItemType::CORPSE, "red"},         {ItemType::ROCK, "gray"},
    {ItemType::PICK_AXE, "white"},     {ItemType::SWORD, "#F7CA88"},
    {ItemType::GOLD_RING, "gold"},     {ItemType::GOLD_COINS, "gold"},
    {ItemType::DAGGER, "gray"},        {ItemType::TORCH, "orange"},
    {ItemType::TORCH_STAND, "orange"},
};

std::map<EnemySpec, std::string> enemySigns = {
    {EnemyType::RAT, "r"}, {EnemyType::BAT, "b"},  {EnemyType::GOBLIN, "g"},
    {EnemyType::ORK, "o"}, {EnemyType::PIXI, "p"}, {EnemyType::OGRE, "O"},
};

std::map<EnemySpec, std::string> enemyColors = {
    {EnemyType::RAT, "gray"},       {EnemyType::BAT, "gray"},
    {EnemyType::GOBLIN, "green"},   {EnemyType::ORK, "#22cc22"},
    {EnemyType::OGRE, "darkgreen"}, {EnemyType::PIXI, "pink"},
};

std::map<CellSpec, std::string> cellSigns = {
    {CellType::FLOOR, "⋅"s},        {CellType::WALL, "#"s},
    {CellType::UNKNOWN_CELL, " "s}, {CellType::DOWNSTAIRS, "&gt;"s},
    {CellType::UPSTAIRS, "&lt;"s},  {CellType::WATER, "="s},
};

std::map<CellSpec, std::map<bool, std::string>> cellColors = {
    {CellType::FLOOR, {{false, "#555"}, {true, "#2f2f2f"}}},
    {CellType::WALL, {{false, "#aaa"}, {true, "#555"}}},
    {CellType::DOWNSTAIRS, {{false, "#aaa"}, {true, "#666"}}},
    {CellType::UPSTAIRS, {{false, "#aaa"}, {true, "#666"}}},
    {CellType::WATER, {{false, "blue"}, {true, "blue"}}}};

std::map<CellSpec, std::string> cellColorsIlluminated = {
    {CellType::FLOOR, "#765"},      {CellType::WALL, "#cba"},
    {CellType::DOWNSTAIRS, "#cba"}, {CellType::UPSTAIRS, "#cba"},
    {CellType::WATER, "#88f"},
};

std::map<CellSpec, std::map<bool, std::string>> cellWeights = {
    {CellType::FLOOR, {{false, "normal"}, {true, "normal"}}},
    {CellType::WALL, {{false, "bold"}, {true, "bold"}}},
    {CellType::DOWNSTAIRS, {{false, "normal"}, {true, "normal"}}},
    {CellType::UPSTAIRS, {{false, "normal"}, {true, "normal"}}},
    {CellType::WATER, {{false, "normal"}, {true, "normal"}}},
};

std::map<std::string, tpl_arg> getCellArgs(std::shared_ptr<Cell> cell) {
  std::string color;
  if (cell->visibilityState == VisibilityState::SEEN) {
    color = cellColors[cell->type][true];
  } else {
    if (cell->illuminated) {
      color = cellColorsIlluminated[cell->type];
    } else {
      color = cellColors[cell->type][false];
    }

    if (cell->hasFeature(CellFeature::CAVE) && cell->type != CellType::WATER) {
      color = "#897546";
    }
    if (cell->hasFeature(CellFeature::BLOOD)) {
      color = "darkred";
    }
    if (cell->hasFeature(CellFeature::MARK1)) {
      color = "blue";
    }
    if (cell->hasFeature(CellFeature::MARK2)) {
      color = "green";
    }
  }
  return {
      {"sign", cellSigns[cell->type]},
      {"color", color},
      {"weight",
       cellWeights[cell->type][cell->visibilityState == VisibilityState::SEEN]},
  };
}

CellSign::CellSign(std::shared_ptr<Cell> cell)
    : Fragment(
          cell->type == CellType::UNKNOWN_CELL ||
                  cell->visibilityState == VisibilityState::UNKNOWN
              ? cellSigns[cell->type]
              : "<span color='{{color}}' weight='{{weight}}'>{{sign}}</span>",
          getCellArgs(cell),
          !(cell->type == CellType::UNKNOWN_CELL ||
            cell->visibilityState == VisibilityState::UNKNOWN)) {}
HeroSign::HeroSign(std::string color)
    : Fragment("<span color='{{color}}' weight='bold'>@</span>",
               {{"color", color}}) {}
EnemySign::EnemySign(EnemySpec type)
    : Fragment("<span color='{{color}}' weight='bold'>{{sign}}</span>",
               {{"sign", enemySigns[type]}, {"color", enemyColors[type]}}) {}
DoorSign::DoorSign(bool opened)
    : Fragment("<span weight='bold'>{{sign}}</span>",
               {{"sign", opened ? "/"s : "+"s}}) {}
ItemSign::ItemSign(ItemSpec type)
    : Fragment("<span color='{{color}}'>{{sign}}</span>",
               {{"sign", itemSigns[type]}, {"color", itemColors[type]}}) {}
