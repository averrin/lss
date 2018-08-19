#include <variant>

#include "Jinja2CppLight.h"
#include "lss/color.hpp"
#include "lss/fragment.hpp"
#include "lss/game/terrain.hpp"
#include "lss/state.hpp"
#include "signs.cpp"

using namespace Jinja2CppLight;
using namespace std::string_literals;

Fragment::Fragment(std::string t, std::map<std::string, tpl_arg> a)
    : template_str(t), args(a) {
  if (args.find("color") != args.end()) {
    fgColor = std::get<std::string>(args["color"]);
  }
};
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

  for (auto [key, value] : args) {
    std::visit([&](auto const &val) { tpl.setValue(key, val); }, value);
  }
  if (fgColor != COLORS::FG) {
    tpl.setValue("color", fgColor);
  }

  auto content = tpl.render();
  if (content != "" && content != "<br>" && content[0] != '<') {
    content = "<span>" + content + "</span>";
  }
  cache = content;
  damaged = false;
  return cache;
}

std::string getColor(std::shared_ptr<Cell> cell) {
  std::string color;
  if (cell->visibilityState == VisibilityState::SEEN) {
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
    }

    // fmt::print("base color: {}\n", color);
    if (cell->illuminated) {
      auto fg = Color(color);
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
        auto lc = Color(lightColors.at((*glow).type));
        fg.blend(lc, 1 / (n + 1));
      }
      color = fg.getString();
    }
    // fmt::print("result color: {}\n", color);
  }
  return color;
}

std::map<std::string, tpl_arg> getCellArgs(std::shared_ptr<Cell> cell) {
  if (cell->type == CellType::UNKNOWN) {
    return {};
  }
  return {
      {"sign", cellSigns.at(cell->type)},
      {"color", getColor(cell)},
      {"weight", cellWeights.at(cell->type)},
  };
}

CellSign::CellSign(std::shared_ptr<Cell> cell)
    : Fragment(cell->type == CellType::UNKNOWN ||
                       cell->visibilityState == VisibilityState::UNKNOWN
                   ? cellSigns.at(cell->type)
                   : "<span color='{{color}}' alpha='{{alpha}}%' "
                     "bgalpha='{{bgAlpha}}%' "
                     "weight='{{weight}}'>{{sign}}</span>",
               getCellArgs(cell),
               !(cell->type == CellType::UNKNOWN ||
                 cell->visibilityState == VisibilityState::UNKNOWN)) {}
HeroSign::HeroSign(std::string color)
    : Fragment("<span color='{{color}}' alpha='100%' bgalpha='{{bgAlpha}}%' "
               "weight='bold'>@</span>",
               {{"color", color}}) {}
EnemySign::EnemySign(EnemySpec type)
    : Fragment(
          "<span color='{{color}}' alpha='{{alpha}}%' bgalpha='{{bgAlpha}}%' "
          "weight='bold'>{{sign}}</span>",
          {{"sign", enemySigns.at(type)}, {"color", enemyColors.at(type)}}) {}
DoorSign::DoorSign(bool opened)
    : Fragment("<span weight='bold' alpha='{{alpha}}%' bgalpha='{{bgAlpha}}%' "
               "color='#8B5F20'>{{sign}}</span>",
               {{"sign", opened ? "/"s : "+"s}}) {}
ItemSign::ItemSign(ItemSpec type)
    : Fragment("<span color='{{color}}' alpha='{{alpha}}%' "
               "bgalpha='{{bgAlpha}}%'>{{sign}}</span>",
               {{"sign", itemSigns.at(type)}, {"color", itemColors.at(type)}}) {
}
TerrainSign::TerrainSign(TerrainSpec type)
    : Fragment("<span color='{{color}}' alpha='{{alpha}}%' "
               "bgalpha='{{bgAlpha}}%'>{{sign}}</span>",
               {{"sign", terrainSigns.at(type)},
                {"color", terrainColors.at(type)}}) {}
