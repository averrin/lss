#include <variant>

#include "Jinja2CppLight.h"
#include "lss/fragment.hpp"
#include "lss/game/terrain.hpp"
#include "lss/state.hpp"
#include "lss/color.hpp"

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
    {TerrainType::MAGIC_LIGHT_LONG, "*"},
    {TerrainType::STATUE, "&amp;"},
    {TerrainType::ALTAR, "_"},
    // {TerrainType::BUSH, "⇞"},
    {TerrainType::BUSH, "❞"},
};

std::map<TerrainSpec, std::string> terrainColors = {
    {TerrainType::TORCH_STAND, "#ffa500"}, {TerrainType::FIREBALL, "#ee1111"},
    {TerrainType::MAGIC_LIGHT, "#add8e6"},
    {TerrainType::MAGIC_LIGHT_LONG, "#add8e6"},
    {TerrainType::STATUE, "#fafafa"},       {TerrainType::ALTAR, "#808080"},
    {TerrainType::BUSH, "#228b22"},
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

    {ItemType::CORPSE, "#ee1111"},          {ItemType::ROCK, "#808080"},
    {ItemType::PICK_AXE, "#fafafa"},      {ItemType::SWORD, "#F7CA88"},
    {ItemType::GOLD_RING, "#ffd700"},      {ItemType::GOLD_COINS, "#ffd700"},
    {ItemType::DAGGER, "#808080"},         {ItemType::TORCH, "#ffa500"},
    {ItemType::GRASS, "#006400"},

    {ItemType::GREAT_AXE, "#fafafa"},     {ItemType::HELMET, "#fafafa"},
    {ItemType::SHIELD, "#fafafa"},        {ItemType::GREAVES, "#fafafa"},
    {ItemType::BOOTS, "#fafafa"},         {ItemType::PLATE, "#fafafa"},
    {ItemType::LEATHER_ARMOR, "#fafafa"}, {ItemType::POTION, "#add8e6"},
    {ItemType::SCROLL, "#fafafa"},
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
    {EnemyType::RAT, "#808080"},
    {EnemyType::BAT, "#808080"},
    {EnemyType::SNAKE, "#808080"},
    {EnemyType::VIPER, "#006400"},
    {EnemyType::BAT_LARGE, "#a9a9a9"},
    {EnemyType::GOBLIN, "#00f000"},
    {EnemyType::ORK, "#22aa22"},
    {EnemyType::ORK_BLACK, "#555"},
    {EnemyType::OGRE, "#006400"},
    {EnemyType::PIXI, "#ff1493"},
    {EnemyType::GOBLIN_LIEUTENANT, "#008b8b"},
    {EnemyType::GOBLIN_ROGUE, "#2f4f4f"},
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

std::map<LightType, std::string> lightColors = {
  {LightType::FIRE, "#cc5511"},
  {LightType::MAGIC, "#1133cc"},
  {LightType::ACID, "#11cc11"},
};

std::map<CellSpec, std::string> cellWeights = {
    {CellType::FLOOR, "normal"},
    {CellType::WALL, "bold"},
    {CellType::DOWNSTAIRS, "bold"},
    {CellType::UPSTAIRS, "bold"},
    {CellType::WATER, "normal"},
    {CellType::VOID, "normal"},
};

std::map<CellFeature, std::string> featureColors = {
  {CellFeature::CAVE, "#897546"},
  {CellFeature::BLOOD, "#8b0000"},
  {CellFeature::MARK1, "#0000ff"},
  {CellFeature::MARK2, "#00ff00"},
};

//FIXME: i thinl multiple blending doesnt work =(
std::map<std::string, tpl_arg> getCellArgs(std::shared_ptr<Cell> cell) {
  std::string color;
  if (cell->visibilityState == VisibilityState::SEEN) {
    color = cellColors[cell->type][true];
  } else {
    color = cellColors[cell->type][false];
    if (cell->illuminated) {
      auto fg = Color(color);
      float n = 0;
      for (auto ls : cell->lightSources) {
        if (!ls->emitsLight) continue;
        n++;
      }
      // fmt::print("{}\n", n);
        // fmt::print("{} -> ", color);
      for (auto ls : cell->lightSources) {
        if (!ls->emitsLight) continue;
        auto lc = Color(lightColors[ls->lightType]);
        fg.blend(lc, 1/(n+1));
        // fmt::print("{} -> ", fg.getString());
      }
      // fmt::print("{}\n", fg.getString());
      color = fg.getString();
    }

    if (cell->type != CellType::VOID) {
      if (cell->hasFeature(CellFeature::CAVE) &&
          cell->type != CellType::WATER) {
        color = featureColors[CellFeature::CAVE];
      }
      if (cell->hasFeature(CellFeature::BLOOD)) {
        color = featureColors[CellFeature::BLOOD];
      }
      if (cell->hasFeature(CellFeature::MARK1)) {
        color = featureColors[CellFeature::MARK1];
      }
      if (cell->hasFeature(CellFeature::MARK2)) {
        color = featureColors[CellFeature::MARK2];
      }
    }
  }
  return {
      {"sign", cellSigns[cell->type]},
      {"color", color},
      // {"alpha", cell->visibilityState == VisibilityState::VISIBLE &&
      // cell->illuminated ? cell->illumination : 60 /*Cell::DEFAULT_LIGHT*/},
      {"weight",
       cellWeights[cell->type]},
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
