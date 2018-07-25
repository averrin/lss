#ifndef __PANGO_H_
#define __PANGO_H_
#include <memory>
#include <SDL.h>

namespace pango {
    typedef std::shared_ptr<class Surface> SurfaceRef;
    class Surface {
    public:
        static SurfaceRef create();

        void setText(std::string text);
        bool render(bool force = false);
        void setDefaultTextColor(SDL_Color color);
    };

}


#endif // __PANGO_H_
