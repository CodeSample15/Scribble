#include <SFML/Graphics.hpp>
#include "runtime/graphics.hpp"

using namespace Graphics;

static std::shared_ptr<sf::RenderWindow> windowPtr = nullptr;

void Graphics::init(uint16_t width, uint16_t height) {
    if(windowPtr != nullptr) windowPtr = nullptr; // deallocate old window if there was one

    windowPtr = std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), "Scribble Project");
}

void Graphics::drawObj(TemplateObjects::Object &obj) {
    if(windowPtr == nullptr) return;
}

void Graphics::renderScreen() {
    if(windowPtr == nullptr) return;
    windowPtr->display();
}

void Graphics::clearScreen() {
    if(windowPtr == nullptr) return;
    windowPtr->clear();
}

void Graphics::pollWindow() {
    if(windowPtr == nullptr) return;

    const std::optional event = windowPtr->pollEvent();
    if (event) {
        if(event->is<sf::Event::Closed>())
            windowPtr->close();
    }
}

bool Graphics::windowOpen() {
    if(windowPtr == nullptr) return false;
    return windowPtr->isOpen();
}

void Graphics::close() {
    if(windowPtr == nullptr) return;
    windowPtr->close();
}