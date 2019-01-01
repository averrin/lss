#ifndef __TEXTGRID_H_
#define __TEXTGRID_H_
#include <map>
#include <SDL.h>
#include <SDLPango.hpp>
#include <fmt/format.h>
#include <liblog/liblog.hpp>

class TextGrid {
    std::map<std::string, pango::SurfaceRef> cache = {};
	SDL_Surface* surface;
    std::vector<std::vector<pango::SurfaceRef>> fragments = {};
	SDL_Renderer* renderer;
    std::vector<std::pair<int, int>> damage = {};

    LibLog::Logger &log = LibLog::Logger::getInstance();
public:
    TextGrid(SDL_Renderer*);
    int width = 0;
    int height = 0;
        void setSize(int w, int h) {
            width = w;
            height = h;
  surface =
      SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_BGRA32);
        }

    void setFragment(int x, int y, std::string f);
    void render();
    void clear();
    void free();

    SDL_Surface *getTexture() {
        return surface;
    }
    
};


#endif // __TEXTGRID_H_
