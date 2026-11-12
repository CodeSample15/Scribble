#pragma once

#include <cstdint>
#include "templates.hpp"

namespace Graphics {
    void init(uint16_t width=800, uint16_t height=600);
    void drawObj(TemplateObjects::Object &obj);
    void renderScreen();
    void clearScreen();
    void pollWindow();

    bool windowOpen();
    void close();
};