#include "lss/palette.hpp"

const Palette palettes::DARK =
    Palette{"dark", SDL_Color{0xcc, 0xcc, 0xcc, 0xFF},
            SDL_Color{0x1d, 0x1f, 0x22, 0xFF},
            SDL_Color{0x0d, 0x0f, 0x12, 0xFF}, "#81b69d", "#cc6666", "#81a2be",
            "#F7CA88", "#555", "#aaa", "#81b69d", "#333", "#666"

    };

const Palette palettes::LIGHT = Palette{"light",
                                        SDL_Color{0x11, 0x11, 0x11, 0xFF},
                                        SDL_Color{0xcc, 0xcc, 0xcc, 0xFF},
                                        SDL_Color{0xbb, 0xbb, 0xbb, 0xFF},
                                        "#137a63",
                                        "#cc6666",
                                        "MidnightBlue",
                                        "#F7CA88",
                                        "#555",
                                        "#111",
                                        "#137a63",
                                        "#777",
                                        "#aaa"};

/*
 // Dark
    @black:           #1d1f21; // 00
    @very-dark-gray:  #282a2e; // 01
    @dark-gray:       #373b41; // 02
    @gray:            #969896; // 03
    @light-gray:      #b4b7b4; // 04
    @very-light-gray: #c5c8c6; // 05
    @almost-white:    #e0e0e0; // 06
    @white:           #ffffff; // 07

    // Colors
    @red:             #cc6666; // 08
    @orange:          #de935f; // 09
    @yellow:          #f0c674; // 0A
    @green:           #b5bd68; // 0B
    @cyan:            #8abeb7; // 0C
    @blue:            #81a2be; // 0D
    @purple:          #b294bb; // 0E
    @brown:           #a3685a; // 0F
 */

/*
 // Light
    @black:           #1d1f21; // 00
    @very-dark-gray:  #282a2e; // 01
    @dark-gray:       #373b41; // 02
    @gray:            #969896; // 03
    @light-gray:      #b4b7b4; // 04
    @very-light-gray: #c5c8c6; // 05
    @almost-white:    #e0e0e0; // 06
    @white:           #ffffff; // 07
 */
