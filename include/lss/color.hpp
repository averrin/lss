#ifndef __COLOR_H_
#define __COLOR_H_
#include <cmath>
#include <fmt/format.h>
#include <sstream>
#include <string>

#include <libcolor/libcolor.hpp>
using namespace LibColor;
using namespace std::string_literals;

namespace COLORS {
const auto FG = "#fafafa"s;
const auto BG = "#0d0f12"s;
const auto WHITE = "#fafafa"s;
const auto GRAY = "#808080"s;
const auto LIGHT_BLUE = "#add8e6"s;
const auto CURSOR = "#5b4650"s;
} // namespace COLORS

#endif // __COLOR_H_
