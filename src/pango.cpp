#include "lss/pango.hpp"

namespace pango {
void Surface::setText(std::string text) {
  fmt::print("setText is not implemented\n");
}
bool Surface::render(bool force) {
  fmt::print("render is not implemented\n");
  return true;
}
void Surface::setDefaultTextColor(SDL_Color color) {

  fmt::print("setDefaultTextColor is not implemented\n");
}

void Surface::setTextAlignment(TextAlignment) {
  fmt::print("setTextAlignment is not implemented\n");
}
void Surface::setMaxSize(float, float) {
  fmt::print("setMaxSize is not implemented\n");
}
void Surface::setMinSize(float, float) {
  fmt::print("setMinSize is not implemented\n");
}
void Surface::setSpacing(float val) {
  fmt::print("setSpacing is not implemented\n");
}
void Surface::disableWrap() { fmt::print("disableWrap is not implemented\n"); }
}
