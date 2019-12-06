#include <variant>

#include "Jinja2CppLight.h"
#include "lss/color.hpp"
#include "lss/game/terrain.hpp"
#include "ui/fragment.hpp"
#include "ui/state.hpp"
#include "signs.cpp"

using namespace Jinja2CppLight;
using namespace std::string_literals;

Fragment::Fragment(std::string t, std::map<std::string, tpl_arg> a)
    : template_str(t), args(a) {
  if (args.find("color") != args.end()) {
    fgColor = std::get<std::string>(args["color"]);
  }
  if (args.find("bgColor") != args.end()) {
    bgColor = std::get<std::string>(args["bgColor"]);
  }
};
Fragment::Fragment(std::string t, std::map<std::string, tpl_arg> a, bool n)
    : template_str(t), args(a), needRender(n){};
Fragment::Fragment(std::string t) : template_str(t){};
Fragment::Fragment(std::string t, bool n) : template_str(t), needRender(n){};

Fragment::~Fragment(){};

std::string Fragment::render(State *state, std::map<std::string, tpl_arg> overrides) {
  if (!needRender && !damaged) {
    if (cache == "") {
      damaged = true;
      render(state);
    }
    return cache;
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
  auto ba = bgAlpha;
  if (ba < 1)
    ba = 1;
  if (ba > 100)
    ba = 100;
  tpl.setValue("alpha", a);
  tpl.setValue("bgAlpha", ba);
  tpl.setValue("green", state->currentPalette.green);
  tpl.setValue("red", state->currentPalette.red);
  tpl.setValue("blue", state->currentPalette.blue);
  tpl.setValue("orange", state->currentPalette.orange);
  tpl.setValue("bgColor", bgColor);

  for (auto [key, value] : args) {
    std::visit([&](auto const &val) { tpl.setValue(key, val); }, value);
  }

  if (fgColor != COLORS::FG) {
    tpl.setValue("color", fgColor);
  }

  if (bgColor != COLORS::BG) {
    tpl.setValue("bgColor", bgColor);
  }

  for (auto [key, value] : overrides) {
    std::visit([&](auto const &val) { tpl.setValue(key, val); }, value);
  }

  auto content = tpl.render();
  if (content != ""s && content != "<br>"s && content[0] != '<') {
    content = "<span>" + content + "</span>";
  }
  cache = content;
  damaged = false;
  return cache;
}

std::string getColor(std::shared_ptr<Cell> cell) {
  std::string color;
  if (cell->visibilityState == VisibilityState::UNKNOWN) {
    color = COLORS::BG;
  } else if (cell->visibilityState == VisibilityState::SEEN) {
    color = cellColors.at(cell->type).at(true);
  } else {
    color = cellColors.at(cell->type).at(false);
    if (cell->type != CellType::VOID) {
      if (cell->hasFeature(CellFeature::CAVE) &&
          cell->type != CellType::WATER) {
        color = featureColors.at(CellFeature::CAVE);
      }
      if (cell->hasFeature(CellFeature::FROST)) {
        color = featureColors.at(CellFeature::FROST);
      }
      if (cell->hasFeature(CellFeature::BLOOD)) {
        color = featureColors.at(CellFeature::BLOOD);
      }
      if (cell->hasFeature(CellFeature::MARK1)) {
        color = featureColors.at(CellFeature::MARK1);
      }
      if (cell->hasFeature(CellFeature::MARK2)) {
        color = featureColors.at(CellFeature::MARK2);
      }
      if (cell->hasFeature(CellFeature::CORRUPT)) {
        color = featureColors.at(CellFeature::CORRUPT);
      }
    }

    // fmt::print("base color: {}\n", color);
    if (cell->illuminated) {
      auto fg = Color::fromHexString(color);
      float n = 0;
      for (auto ls : cell->lightSources) {
        auto glow = ls->getGlow();
        if (!glow)
          continue;
        n++;
      }
      for (auto ls : cell->lightSources) {
        auto glow = ls->getGlow();
        if (!glow)
          continue;
        auto lc = Color::fromHexString(lightColors.at((*glow).type));
        fg.blend(lc, 1 / (n + 1));
      }
      color = fg.hex();
    }
    // fmt::print("result color: {}\n", color);
  }
  return color;
}

std::tuple<std::string, int> getBgColor(std::shared_ptr<Cell> cell) {
  // if (cell->trigger && cell->visibilityState == VisibilityState::VISIBLE) {
  //   return {"#aae", 40};
  // }
  if (cell->hasFeature(CellFeature::ACID)) {
    return {"#2da02f", 70};
  }
  if (cell->hasFeature(CellFeature::BLOOD)) {
    return {"#602d2f", 50};
  }
  if (cell->hasFeature(CellFeature::FROST)) {
    return {"#2d2f60", 50};
  }
  if (cell->type == CellType::WATER) {
    return {"#8d8fc0", 50};
  }

  if (cell->hasFeature(CellFeature::CORRUPT)) {
    return {"#8800aa", 50};
  }

  return {COLORS::BG, 100};
}

std::map<std::string, tpl_arg> getCellArgs(std::shared_ptr<Cell> cell) {
  auto [bgc, bga] = getBgColor(cell);
  return {
      {"sign", cellSigns.at(cell->type)},
      {"color", getColor(cell)},
      {"bgColor", bgc},
      {"bgAlpha", bga},
      {"weight", cellWeights.at(cell->type)},
  };
}

CellSign::CellSign(std::shared_ptr<Cell> cell)
    : Fragment("<span color='{{color}}' alpha='{{alpha}}%' "
                     "bgcolor='{{bgColor}}' "
                     "bgalpha='{{bgAlpha}}%' "
                     "weight='{{weight}}'>{{sign}}</span>",
               getCellArgs(cell),
               !(cell->type == CellType::UNKNOWN ||
                 cell->visibilityState == VisibilityState::UNKNOWN)) {}
HeroSign::HeroSign(std::shared_ptr<Cell> cell, std::string color)
    : Fragment("<span color='{{color}}' alpha='100%' bgalpha='{{bgAlpha}}%' "
               "bgcolor='{{bgColor}}' "
               "weight='bold'>@</span>",
               {
                   {"color", color},
                   {"bgColor", std::get<0>(getBgColor(cell))},
                   {"bgAlpha", std::get<1>(getBgColor(cell))},
               }) {}
EnemySign::EnemySign(std::shared_ptr<Cell> cell, EnemySpec type)
    : Fragment(
          "<span color='{{color}}' alpha='{{alpha}}%' bgalpha='{{bgAlpha}}%' "
          "bgcolor='{{bgColor}}' "
          "weight='bold'>{{sign}}</span>",
          {
              {"sign", enemySigns.at(type)},
              {"color", enemyColors.at(type)},
              {"bgColor", std::get<0>(getBgColor(cell))},
              {"bgAlpha", std::get<1>(getBgColor(cell))},
          }) {}
DoorSign::DoorSign(bool opened)
    : Fragment("<span weight='bold' alpha='{{alpha}}%' bgalpha='{{bgAlpha}}%' "
               "bgcolor='{{bgColor}}' "
               "color='#8B5F20'>{{sign}}</span>",
               {{"sign", opened ? "/"s : "+"s}}) {}
ItemSign::ItemSign(std::shared_ptr<Cell> cell, ItemSpec type)
    : Fragment("<span color='{{color}}' alpha='{{alpha}}%' "
               "bgcolor='{{bgColor}}' "
               "bgalpha='{{bgAlpha}}%'>{{sign}}</span>",
               {
                   {"sign", itemSigns.at(type)},
                   {"color", itemColors.at(type)},
                   {"bgColor", std::get<0>(getBgColor(cell))},
                   {"bgAlpha", std::get<1>(getBgColor(cell))},
               }) {}
TerrainSign::TerrainSign(std::shared_ptr<Cell> cell, TerrainSpec type)
    : Fragment("<span color='{{color}}' alpha='{{alpha}}%' "
               "bgcolor='{{bgColor}}' "
               "bgalpha='{{bgAlpha}}%'>{{sign}}</span>",
               {{"sign", terrainSigns.find(type) != terrainSigns.end()
                             ? terrainSigns.at(type)
                             : cellSigns.at(cell->type)},
                {"bgColor", std::get<0>(getBgColor(cell))},
                {"bgAlpha", std::get<1>(getBgColor(cell))},
                {"color", terrainColors.at(type)}}) {}
