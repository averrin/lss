#include "lss/pango.hpp"
#include <regex>

namespace pango {
SurfaceRef Surface::create() {
	return SurfaceRef(new Surface())->shared_from_this();
}

    Surface::Surface() {
        fmt::print("pango create called\n");
        context = SDLPango_CreateContext();

        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
        #else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
        #endif

        SDL_Color color{0xff,0xff,0xff};
        SDLPango_SetColor(context, &color);
    }
    Surface::~Surface() {
        SDLPango_FreeContext(context);
        SDL_FreeSurface(surface);
    }

    SDL_Surface* Surface::getTexture() {
        return surface;
    }

void Surface::setText(std::string text) {
  text = "<span color='red'>wtf?!</span>   <br> bbb";

  std::regex e("<br\\s?/?>", std::regex_constants::icase);
  text = std::regex_replace(text, e, "\n");
  SDLPango_SetMarkup(context, text.c_str(), -1);
}
bool Surface::render(bool force) {
  // fmt::print("pango render called\n");
  SDLPango_Draw(context, surface, 0, 0);
  return true;
}
void Surface::setDefaultTextColor(SDL_Color color) {
  SDLPango_SetColor(context, &color);
}

void Surface::setTextAlignment(TextAlignment) {
  fmt::print("setTextAlignment is not implemented\n");
}
void Surface::setMaxSize(float, float) {
  fmt::print("setMaxSize is not implemented\n");
}
void Surface::setMinSize(float w, float h) {
  width = w;
  height = h;
  SDLPango_SetMinimumSize(context, w, h);
  SDL_FreeSurface(surface);
  surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
}
void Surface::setSpacing(float val) {
  fmt::print("setSpacing is not implemented\n");
}
void Surface::disableWrap() { fmt::print("disableWrap is not implemented\n"); }
}
