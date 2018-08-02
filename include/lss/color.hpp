#ifndef __COLOR_H_
#define __COLOR_H_
#include <string>
#include <sstream>
#include <cmath>
#include <fmt/format.h>

class Color {
public:
    Color(std::string name) {
        // fmt::print("{}->", name);
        if (name.front() != '#') return;
        name = name.substr(1, name.size() - 1);
        // fmt::print("{}->", name);
        if (name.size() == 3) {
            name = fmt::format("{}{}{}{}{}{}",
                name[0] , name[0] ,name[1] , name[1] ,name[2] , name[2]);
        }
        // fmt::print("{}->", name);
        std::stringstream st;
        st << name;
        int color;
        st >> std::hex >> color;
        // fmt::print("{}\n", color);
        r = (color & 0xff0000) >> 16;
        g = (color & 0x00ff00) >> 8;
        b = (color & 0x0000ff);
    }
    int r = 0;
    int g = 0;
    int b = 0;

    std::string getString() {
        char hexcol[16];
        snprintf(hexcol, sizeof hexcol, "#%02x%02x%02x", r, g, b);
        return hexcol;
    }

    void blend(Color color, float k = 0.5) {
        r = r * (1-k) + color.r * k;
        g = g * (1-k) + color.g * k;
        b = b * (1-k) + color.b * k;

        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;
    }
};


#endif // __COLOR_H_
