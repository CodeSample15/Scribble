#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include "templates.hpp"

namespace Graphics {
    extern uint16_t ScreenWidth, ScreenHeight;

    void init(uint16_t width=800, uint16_t height=600);
    void drawObj(TemplateObjects::GameObject* obj);
    void renderScreen();
    void clearScreen();
    void pollWindow();
    bool windowOpen();
    void close();

    sf::Vector2i getMouseWindowLoc();
};