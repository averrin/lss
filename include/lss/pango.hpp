#ifndef __PANGO_H_
#define __PANGO_H_
#include <SDL.h>
#include <fmt/format.h>
#include <memory>

namespace pango {
enum class TextRenderer { FREETYPE };
enum class TextAlignment { LEFT, RIGHT, CENTER };

typedef std::shared_ptr<class Surface> SurfaceRef;
class Surface {
public:
    Surface() {}
    ~Surface() {}
    static SurfaceRef create() {
        return std::shared_ptr<Surface>();
    }

  void setText(std::string text);
  void setTextAlignment(TextAlignment);
  void setMaxSize(float, float);
  void setMinSize(float, float);
  void setSpacing(float val);
  void disableWrap();
  bool render(bool force = false);
  void setDefaultTextColor(SDL_Color color);
};
}

#endif // __PANGO_H_
