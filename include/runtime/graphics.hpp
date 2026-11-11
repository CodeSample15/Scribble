#pragma once

#include <cstdint>
#include "templates.hpp"

namespace Graphics {
    enum ImageShape {
        CIRCLE,
        SQUARE,
        TRIANGLE,
        PENTAGON,
        HEXAGON,
        OCTAGON,
        CUSTOM
    };

    void init(uint16_t width, uint16_t height);
    void drawObj(TemplateObjects::Object &obj);
    void renderScreen();

    bool windowOpen();
    void close();
};