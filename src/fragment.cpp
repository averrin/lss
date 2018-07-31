#include <variant>

#include "Jinja2CppLight.h"
#include "lss/fragment.hpp"
#include "lss/game/terrain.hpp"
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

  auto a = alpha;
  if (a < 1)
    a = 1;
  if (a > 100)
    a = 100;
  tpl.setValue("alpha", a);
  tpl.setValue("green", state->currentPalette.green);
  tpl.setValue("red", state->currentPalette.red);
  tpl.setValue("blue", state->currentPalette.blue);
  tpl.setValue("orange", state->currentPalette.orange);

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

std::map<TerrainSpec, std::string> terrainSigns = {
    {TerrainType::TORCH_STAND, "*"},
    {TerrainType::FIREBALL, "*"},
    {TerrainType::MAGIC_LIGHT, "*"},
    {TerrainType::STATUE, "&amp;"},
    {TerrainType::ALTAR, "_"},
    // {TerrainType::BUSH, "⇞"},
    {TerrainType::BUSH, "❞"},
};

std::map<TerrainSpec, std::string> terrainColors = {
    {TerrainType::TORCH_STAND, "orange"}, {TerrainType::FIREBALL, "red"},
    {TerrainType::MAGIC_LIGHT, "lightblue"},
    {TerrainType::STATUE, "white"},       {TerrainType::ALTAR, "gray"},
    {TerrainType::BUSH, "forestgreen"},
};

std::map<ItemSpec, std::string> itemSigns = {
    {ItemType::CORPSE, "%"},        {ItemType::ROCK, "*"},
    {ItemType::PICK_AXE, "("},      {ItemType::SWORD, "("},
    {ItemType::GOLD_RING, "="},     {ItemType::GOLD_COINS, "$"},
    {ItemType::DAGGER, "("},        {ItemType::TORCH, "]"},
    {ItemType::GRASS, "\""},

    {ItemType::GREAT_AXE, "("},     {ItemType::HELMET, "["},
    {ItemType::SHIELD, "["},        {ItemType::GREAVES, "["},
    {ItemType::BOOTS, "["},         {ItemType::PLATE, "["},
    {ItemType::LEATHER_ARMOR, "["}, {ItemType::POTION, "!"},
    {ItemType::SCROLL, "?"},        {ItemType::BONES, "%"},
};

std::map<ItemSpec, std::string> itemColors = {
    {ItemType::BONES, "#aaa"},

    {ItemType::CORPSE, "red"},          {ItemType::ROCK, "gray"},
    {ItemType::PICK_AXE, "white"},      {ItemType::SWORD, "#F7CA88"},
    {ItemType::GOLD_RING, "gold"},      {ItemType::GOLD_COINS, "gold"},
    {ItemType::DAGGER, "gray"},         {ItemType::TORCH, "orange"},
    {ItemType::GRASS, "darkgreen"},

    {ItemType::GREAT_AXE, "white"},     {ItemType::HELMET, "white"},
    {ItemType::SHIELD, "white"},        {ItemType::GREAVES, "white"},
    {ItemType::BOOTS, "white"},         {ItemType::PLATE, "white"},
    {ItemType::LEATHER_ARMOR, "white"}, {ItemType::POTION, "lightblue"},
    {ItemType::SCROLL, "white"},
};

std::map<EnemySpec, std::string> enemySigns = {
    {EnemyType::RAT, "r"},
    {EnemyType::BAT, "b"},
    {EnemyType::SNAKE, "s"},
    {EnemyType::VIPER, "s"},
    {EnemyType::BAT_LARGE, "B"},
    {EnemyType::GOBLIN, "g"},
    {EnemyType::ORK_BLACK, "o"},
    {EnemyType::ORK, "o"},
    {EnemyType::PIXI, "p"},
    {EnemyType::OGRE, "O"},
    {EnemyType::GOBLIN_LIEUTENANT, "g"},
    {EnemyType::GOBLIN_ROGUE, "g"},
};

std::map<EnemySpec, std::string> enemyColors = {
    {EnemyType::RAT, "gray"},
    {EnemyType::BAT, "gray"},
    {EnemyType::SNAKE, "gray"},
    {EnemyType::VIPER, "darkgreen"},
    {EnemyType::BAT_LARGE, "darkgray"},
    {EnemyType::GOBLIN, "green"},
    {EnemyType::ORK, "#22aa22"},
    {EnemyType::ORK_BLACK, "#555"},
    {EnemyType::OGRE, "darkgreen"},
    {EnemyType::PIXI, "pink"},
    {EnemyType::GOBLIN_LIEUTENANT, "#008b8b"},
    {EnemyType::GOBLIN_ROGUE, "darkslategray"},
};

std::map<CellSpec, std::string> cellSigns = {
    {CellType::FLOOR, "⋅"s},
    {CellType::WALL, "#"s},
    {CellType::UNKNOWN, " "s},
    {CellType::DOWNSTAIRS, "&gt;"s},
    {CellType::UPSTAIRS, "&lt;"s},
    {CellType::WATER, "="s},
    // {CellType::VOID, "◦"s},
    {CellType::VOID, "⌄"s},
};

std::map<CellSpec, std::map<bool, std::string>> cellColors = {
    {CellType::FLOOR, {{false, "#555"}, {true, "#2f2f2f"}}},
    {CellType::WALL, {{false, "#aaa"}, {true, "#555"}}},
    {CellType::DOWNSTAIRS, {{false, "#aaa"}, {true, "#666"}}},
    {CellType::UPSTAIRS, {{false, "#aaa"}, {true, "#666"}}},
    {CellType::WATER, {{false, "#5e589e"}, {true, "#557"}}},
    {CellType::VOID, {{false, "#555"}, {true, "#555"}}}};

std::map<CellSpec, std::string> cellColorsIlluminated = {
    {CellType::FLOOR, "#875"},      {CellType::WALL, "#eca"},
    {CellType::DOWNSTAIRS, "#cba"}, {CellType::UPSTAIRS, "#cba"},
    {CellType::WATER, "#77f"},      {CellType::VOID, "#555"},
};

std::map<CellSpec, std::map<bool, std::string>> cellWeights = {
    {CellType::FLOOR, {{false, "normal"}, {true, "normal"}}},
    {CellType::WALL, {{false, "bold"}, {true, "bold"}}},
    {CellType::DOWNSTAIRS, {{false, "bold"}, {true, "bold"}}},
    {CellType::UPSTAIRS, {{false, "bold"}, {true, "bold"}}},
    {CellType::WATER, {{false, "normal"}, {true, "normal"}}},
    {CellType::VOID, {{false, "normal"}, {true, "normal"}}},
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

    if (cell->type != CellType::VOID) {
      if (cell->hasFeature(CellFeature::CAVE) &&
          cell->type != CellType::WATER) {
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
  }
  return {
      {"sign", cellSigns[cell->type]},
      {"color", color},
      // {"alpha", cell->visibilityState == VisibilityState::VISIBLE &&
      // cell->illuminated ? cell->illumination : 60 /*Cell::DEFAULT_LIGHT*/},
      {"weight",
       cellWeights[cell->type][cell->visibilityState == VisibilityState::SEEN]},
  };
}

CellSign::CellSign(std::shared_ptr<Cell> cell)
    : Fragment(cell->type == CellType::UNKNOWN ||
                       cell->visibilityState == VisibilityState::UNKNOWN
                   ? cellSigns[cell->type]
                   : "<span color='{{color}}' alpha='{{alpha}}%' "
                     "weight='{{weight}}'>{{sign}}</span>",
               getCellArgs(cell),
               !(cell->type == CellType::UNKNOWN ||
                 cell->visibilityState == VisibilityState::UNKNOWN)) {}
HeroSign::HeroSign(std::string color)
    : Fragment("<span color='{{color}}' alpha='100%' weight='bold'>@</span>",
               {{"color", color}}) {}
EnemySign::EnemySign(EnemySpec type)
    : Fragment("<span color='{{color}}' alpha='{{alpha}}%' "
               "weight='bold'>{{sign}}</span>",
               {{"sign", enemySigns[type]}, {"color", enemyColors[type]}}) {}
DoorSign::DoorSign(bool opened)
    : Fragment("<span weight='bold' alpha='{{alpha}}%' "
               "color='#8B5F20'>{{sign}}</span>",
               {{"sign", opened ? "/"s : "+"s}}) {}
ItemSign::ItemSign(ItemSpec type)
    : Fragment("<span color='{{color}}' alpha='{{alpha}}%'>{{sign}}</span>",
               {{"sign", itemSigns[type]}, {"color", itemColors[type]}}) {}
TerrainSign::TerrainSign(TerrainSpec type)
    : Fragment("<span color='{{color}}' alpha='{{alpha}}%'>{{sign}}</span>",
               {{"sign", terrainSigns[type]}, {"color", terrainColors[type]}}) {
}
