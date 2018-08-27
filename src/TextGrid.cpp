#include <TextGrid.hpp>

const int w = 12;
const int h = 20;

//TODO: draw objects over cells

TextGrid::TextGrid(SDL_Renderer *r) : renderer(r) {
  surface =
      SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_BGRA32);
}

void TextGrid::setFragment(int x, int y, std::string f) {
  while (fragments.size() == 0 || y > fragments.size() - 1) {
    fragments.push_back({});
  }
  while (fragments[y].size() == 0 || x > fragments[y].size() - 1) {
    fragments[y].push_back(nullptr);
  }
  pango::SurfaceRef s;
  if (cache.find(f) != cache.end()) {
    fragments[y][x] = cache[f];
  } else {
    auto s = pango::Surface::create(renderer);
    s->setDefaultTextColor(SDL_Color{0xcc, 0xcc, 0xcc, 0xFF});
    s->setMaxSize(w, h);
    s->setText(fmt::format("<tt>{}</tt>", f));
    fragments[y][x] = s;
    cache[f] = s;
  }
  damage.push_back({x, y});
}

void TextGrid::render() {
  for (auto [x, y] : damage) {
    auto s = fragments[y][x];
    s->render();
    //TODO: fix mirrored surface
    SDL_Rect dst = {(x-1) * w, height - (y+1) * h, w, h};
    auto c = SDL_MapRGB(surface->format, 0x0d, 0x0f, 0x12);
    SDL_FillRect(surface, &dst, c);
    SDL_BlitSurface(s->getTexture(), NULL, surface, &dst);
  }
  // fmt::print("{} fragments updated\n", damage.size());
  damage.clear();
}

void TextGrid::clear() {
  auto c = SDL_MapRGB(surface->format, 0x0d, 0x0f, 0x12);
  SDL_FillRect(surface, NULL, c);
}

void TextGrid::free() {
  if (surface == nullptr)
    return;
  SDL_FreeSurface(surface);
  surface = nullptr;
  for (auto r : fragments) {
      for (auto s : r) {
          if (s == nullptr) continue;
          s->free();
      }
  }
  for (auto [_, cs] : cache) {
    if (cs == nullptr) continue;
      cs->free();
  }
}
