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

    int width = 640;
    int height = 200;
    int margin = 10;

    AppState() {
        // Initial values
        m_sdlWindow = NULL;
        m_sdlGLContextDrawing = NULL;
        m_renderWidth = 0;
        m_renderHeight = 0;

        // Initialize SDL
        int rc = SDL_Init(SDL_INIT_VIDEO);
        assert(rc >= 0);
        (void) rc;

        // Set SDL GL attributes
        // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
    
        // Create SDL window
        m_sdlWindow = SDL_CreateWindow("Long Story Short", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                       width, height, SDL_WINDOW_OPENGL);
        assert(m_sdlWindow);
        SDL_ShowWindow(m_sdlWindow);

        renderer = SDL_CreateRenderer(m_sdlWindow, -1, SDL_RENDERER_ACCELERATED);

        context = SDLPango_CreateContext();

        SDLPango_SetColor(context, &textcolor);
        SDLPango_SetMinimumSize(context, width - margin, height - margin);

        SDLPango_SetMarkup(context, "boom <b>1111</b>", -1);

        // Create SDL GL context
        // m_sdlGLContextDrawing = SDL_GL_CreateContext(m_sdlWindow);
        // assert(m_sdlGLContextDrawing);
        // SDL_GL_MakeCurrent(m_sdlWindow, m_sdlGLContextDrawing);
        // SDL_GL_SetSwapInterval(-1);

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
        SDL_FreeSurface(surface);

        SDL_SetRenderDrawColor(renderer, 0x1d, 0x1f, 0x22, 0xff);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, background, NULL, NULL);

#ifdef _WIN32
        // Initialize GLES2 function table
        glFuncTable.initialize();
#endif

        // Get render buffer width/height
        SDL_GL_GetDrawableSize(m_sdlWindow, &m_renderWidth, &m_renderHeight);

        // Log GL driver info
        SDL_Log("Vendor     : %s\n", GL_NO_CHECK(GetString(GL_VENDOR)));
        SDL_Log("Renderer   : %s\n", GL_NO_CHECK(GetString(GL_RENDERER)));
        SDL_Log("Version    : %s\n", GL_NO_CHECK(GetString(GL_VERSION)));
        // SDL_Log("Extensions : %s\n", GL_NO_CHECK(GetString(GL_EXTENSIONS)));
        SDL_Log("\n");
    }

    ~AppState() {
        SDLPango_FreeContext(context);
        // SDL_GL_MakeCurrent(NULL, NULL);
        SDL_DestroyWindow(m_sdlWindow);
        // SDL_GL_DeleteContext(m_sdlGLContextDrawing);
        SDL_Quit();
    }
};


//---------------------------------------------------------------------------
//  View: Manages the stuff to be rendered
//---------------------------------------------------------------------------
class View {
public:
    AppState* m_appState;

    // GL resources
    GLuint m_shaderID;
    GLint m_vertPositionAttrib;
    GLint m_vertNormalAttrib;
    GLint m_modelToCameraAttrib;
    GLint m_cameraToViewportAttrib;
    GLint m_colorAttrib;

    View(AppState *appState) : m_appState(appState) {
    }

    ~View() {
    }

    void update() {

        SDLPango_SetMarkup(m_appState->context, fmt::format("boom <b>{}</b>", rand() % 5).c_str(), -1);
        SDLPango_Draw(m_appState->context, m_appState->surface, m_appState->margin, m_appState->margin);

        // Clear viewport
        // GL_CHECK(Viewport(0, 0, (GLsizei) m_appState->m_renderWidth, (GLsizei) m_appState->m_renderHeight));
        // GL_CHECK(ClearColor(0.9f, 0.9f, 0.9f, 1));
        // GL_CHECK(Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

        // // Disable alpha blend
        // GL_CHECK(Disable(GL_BLEND));

        // // Enable depth test
        // GL_CHECK(Enable(GL_DEPTH_TEST));
        // GL_CHECK(DepthMask(GL_TRUE));

        // // Enable face culling
        // GL_CHECK(Enable(GL_CULL_FACE));
        // GL_CHECK(CullFace(GL_BACK));
        // GL_CHECK(FrontFace(GL_CCW));

        // // Present framebuffer
        SDL_RenderPresent(m_appState->renderer);
        // SDL_GL_SwapWindow(m_appState->m_sdlWindow);
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
