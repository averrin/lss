#include "lss/palette.hpp"
using namespace std::string_literals;

const Palette palettes::DARK = Palette{"dark"s,
                                       SDL_Color{0xcc, 0xcc, 0xcc, 0xFF},
                                       SDL_Color{0x1d, 0x1f, 0x22, 0xFF},
                                       SDL_Color{0x0d, 0x0f, 0x12, 0xFF},
                                       "#81b69d"s,
                                       "#cc6666"s,
                                       "#81a2be"s,
                                       "#F7CA88"s,
                                       "#555"s,
                                       "#aaa"s,
                                       "#81b69d"s,
                                       "#333"s,
                                       "#666"s

};

const Palette palettes::LIGHT = Palette{"light"s,
                                        SDL_Color{0x11, 0x11, 0x11, 0xFF},
                                        SDL_Color{0xcc, 0xcc, 0xcc, 0xFF},
                                        SDL_Color{0xbb, 0xbb, 0xbb, 0xFF},
                                        "#137a63"s,
                                        "#cc6666"s,
                                        "MidnightBlue"s,
                                        "#F7CA88"s,
                                        "#555"s,
                                        "#111"s,
                                        "#137a63"s,
                                        "#777"s,
                                        "#aaa"s};

