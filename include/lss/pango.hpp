#ifndef __PANGO_H_
#define __PANGO_H_
#include <SDL.h>
#include <fmt/format.h>
#include <memory>
#include <SDL_Pango.hpp>

namespace pango {
enum class TextRenderer { FREETYPE };
enum class TextAlignment { LEFT, RIGHT, CENTER };

typedef std::shared_ptr<class Surface> SurfaceRef;
class Surface : public std::enable_shared_from_this<Surface>{
public:
    SDLPango_Context *context = nullptr;
    SDL_Surface *surface = nullptr;
    float width = 0;
    float height = 0;
    Uint32 rmask, gmask, bmask, amask;

    Surface();
    ~Surface();
    static SurfaceRef create();

  void setText(std::string text);
  void setTextAlignment(TextAlignment);
  void setMaxSize(float, float);
  void setMinSize(float, float);
  void setSpacing(float val);
  void disableWrap();
  bool render(bool force = false);
  void setDefaultTextColor(SDL_Color color);
  SDL_Surface* getTexture();
};
}

#endif // __PANGO_H_
