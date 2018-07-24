#include <SDL.h>
#include <SDL_opengl.h>
#include <pango/pangocairo.h>
#include <string>

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
std::string FONT = "Sans Bold 18";
std::string TEXT = "The quick brown fox is so かわいい!";

void init_sdl() {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_OPENGL);
}

void init_gl() {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

unsigned int create_texture(unsigned int width, unsigned int height,
                            unsigned char *pixels) {
  unsigned int texture_id;

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA,
               GL_UNSIGNED_BYTE, pixels);

  return texture_id;
}

void draw_texture(int width, int height, unsigned int texture_id) {
  /* Render a texture in immediate mode. */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT);
  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glColor3f(1.f, 1.0f, 1.0f);

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(0.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(width, 0.0f);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(width, height);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(0.0f, height);
  glEnd();

  glPopMatrix();
  SDL_GL_SwapBuffers();
}

cairo_t *create_cairo_context(int width, int height, int channels,
                              cairo_surface_t **surf, unsigned char **buffer) {
  *buffer =
      (unsigned char *)calloc(channels * width * height, sizeof(unsigned char));
  *surf = cairo_image_surface_create_for_data(*buffer, CAIRO_FORMAT_ARGB32,
                                              width, height, channels * width);
  return cairo_create(*surf);
}

cairo_t *create_layout_context() {
  cairo_surface_t *temp_surface;
  cairo_t *context;

  temp_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
  context = cairo_create(temp_surface);
  cairo_surface_destroy(temp_surface);

  return context;
}

void get_text_size(PangoLayout *layout, unsigned int *width,
                   unsigned int *height) {
  pango_layout_get_size(layout, (int *)width, (int *)height);
  /* Divide by pango scale to get dimensions in pixels. */
  *width /= PANGO_SCALE;
  *height /= PANGO_SCALE;
}

void render_text(const char *text, unsigned int *text_width,
                 unsigned int *text_height, unsigned int *texture_id) {
  cairo_t *layout_context;
  cairo_t *render_context;
  cairo_surface_t *temp_surface;
  cairo_surface_t *surface;
  unsigned char *surface_data = NULL;
  PangoFontDescription *desc;
  PangoLayout *layout;

  layout_context = create_layout_context();

  /* Create a PangoLayout, set the font and text */
  layout = pango_cairo_create_layout(layout_context);
  pango_layout_set_text(layout, text, -1);

  /* Load the font */
  desc = pango_font_description_from_string(FONT.c_str());
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  /* Get text dimensions and create a context to render to */
  get_text_size(layout, text_width, text_height);
  render_context = create_cairo_context(*text_width, *text_height, 4, &surface,
                                        &surface_data);

  /* Render */
  cairo_set_source_rgba(render_context, 1, 1, 1, 1);
  pango_cairo_show_layout(render_context, layout);

  *texture_id = create_texture(*text_width, *text_height, surface_data);

  /* Clean up */
  free(surface_data);
  g_object_unref(layout);
  cairo_destroy(layout_context);
  cairo_destroy(render_context);
  cairo_surface_destroy(surface);
}

int main(int argc, char **argv) {
  SDL_Event event;
  int keep_running = 1;
  unsigned int texture_id;
  unsigned int text_width = 0;
  unsigned int text_height = 0;

  init_sdl();
  init_gl();
  render_text(TEXT.c_str(), &texture_id, &text_width, &text_height);

  /* Update/render loop */
  while (keep_running) {
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
      keep_running = 0;
      break;

    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE)
        keep_running = 0;
      if (event.key.keysym.sym == SDLK_q)
        keep_running = 0;
      break;
    }

    draw_texture(texture_id, text_width, text_height);
    SDL_Delay(16);
  }

  /* Clean up */
  glDeleteTextures(1, &texture_id);

  SDL_Quit();

  return 0;
}
