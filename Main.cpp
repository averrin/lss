#include <SDL.h>
#include <math.h>
#include <assert.h>
#include <vector>
#include <algorithm>
#include "gl2/Context.h"
#include <demo-config.h>
#include <SDL_Pango.hpp>
#include <fmt/format.h>
#include <random>

static const float Pi = 3.14159265358979323846f;


//---------------------------------------------------------------------------
//  AppState: Initializes & shuts down SDL
//---------------------------------------------------------------------------
class AppState {
public:
    SDL_Window* m_sdlWindow;
    SDL_GLContext m_sdlGLContextDrawing;
    int m_renderWidth;
    int m_renderHeight;

    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture* background;
    Uint32 rmask, gmask, bmask, amask;
    SDLPango_Context *context;
    SDL_Color textcolor = {0xcc, 0xcc, 0xcc, 0xff};

    int width = 800;
    int height = 600;
    int margin = 10;

    AppState() {
        // Initial values
        m_sdlWindow = nullptr;
        m_sdlGLContextDrawing = nullptr;
        m_renderWidth = 0;
        m_renderHeight = 0;

        // Initialize SDL
        int rc = SDL_Init(SDL_INIT_VIDEO);
        assert(rc >= 0);
        (void) rc;

        // Create SDL window
        m_sdlWindow = SDL_CreateWindow("Long Story Short", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                       width, height, SDL_WINDOW_OPENGL);
        assert(m_sdlWindow);
        SDL_ShowWindow(m_sdlWindow);

        renderer = SDL_CreateRenderer(m_sdlWindow, -1, SDL_RENDERER_ACCELERATED);

        context = SDLPango_CreateContext();

        SDLPango_SetColor(context, &textcolor);
        SDLPango_SetMinimumSize(context, width - margin, height - margin);

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

        surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);

        SDLPango_Draw(context, surface, margin, margin);

        background = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_SetRenderDrawColor(renderer, 0x1d, 0x1f, 0x22, 0xff);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, background, NULL, NULL);

#ifdef _WIN32
        // Initialize GLES2 function table
        glFuncTable.initialize();
#endif

        // Log GL driver info
        SDL_Log("Vendor     : %s\n", GL_NO_CHECK(GetString(GL_VENDOR)));
        SDL_Log("Renderer   : %s\n", GL_NO_CHECK(GetString(GL_RENDERER)));
        SDL_Log("Version    : %s\n", GL_NO_CHECK(GetString(GL_VERSION)));
        // SDL_Log("Extensions : %s\n", GL_NO_CHECK(GetString(GL_EXTENSIONS)));
        SDL_Log("\n");
    }

    ~AppState() {
        SDLPango_FreeContext(context);
        SDL_FreeSurface(surface);
        SDL_DestroyWindow(m_sdlWindow);
        SDL_Quit();
    }
};


//---------------------------------------------------------------------------
//  View: Manages the stuff to be rendered
//---------------------------------------------------------------------------
class View {
public:
    AppState* m_appState;

    View(AppState *appState) : m_appState(appState) {
    }

    ~View() {
    }

    void update() {

        SDLPango_SetMarkup(m_appState->context, fmt::format("boom <b>{}</b>", rand() % 500).c_str(), -1);
        SDLPango_Draw(m_appState->context, m_appState->surface, m_appState->margin, m_appState->margin);
        m_appState->background = SDL_CreateTextureFromSurface(m_appState->renderer, m_appState->surface);

        SDL_RenderClear(m_appState->renderer);

        SDL_RenderCopy(m_appState->renderer, m_appState->background, NULL, NULL);

        SDL_RenderPresent(m_appState->renderer);
    }
};


//---------------------------------------------------------------------------
//  Main loop
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    // Initialize
    AppState* appState = new AppState;
    View* view = new View(appState);

    // Main loop
    Uint32 lastTick = SDL_GetTicks();
    bool running = true;
    while (running) {
        Uint32 now = SDL_GetTicks();
        if (now - lastTick < 16) {
            SDL_Delay(16 - (now - lastTick));
            now = SDL_GetTicks();
        }
        lastTick = now;

        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                // case SDL_MOUSEBUTTONDOWN:
                // case SDL_MOUSEBUTTONUP: {
                //     if (event.button.button == SDL_BUTTON_LEFT) {
                //         view->m_rotationRateTarget = (event.type == SDL_MOUSEBUTTONDOWN) ? 0.f : 0.8f;
                //     }
                //     break;
                // }

                // case SDL_MOUSEMOTION: {
                //     if ((event.motion.state & SDL_BUTTON_LMASK) != 0) {
                //         view->m_yaw += event.motion.xrel * 0.01f;
                //         view->m_pitch += event.motion.yrel * 0.01f;
                //         view->m_pitch = std::max(std::min(view->m_pitch, Pi / 2), -Pi / 2);
                //     }
                //     break;
                // }

                case SDL_WINDOWEVENT: {
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        running = false;
                    }
                    break;
                }
            }
        }

        // Update the view
        if (running) {
            view->update();
        }
    }

    // Shutdown
    delete view;
    delete appState;
    return 0;
}
