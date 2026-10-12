#pragma once

#include <cstdint>

/*
    Images represent what an Object looks like when rendered to the screen.
    This can be a one of the built-in shapes, or an image loaded from disk.

    This object provides an interface to render an Image to the screen.
*/

enum ImageShape {
    CIRCLE,
    SQUARE,
    TRIANGLE,
    PENTAGON,
    HEXAGON,
    OCTAGON,
    CUSTOM
};

struct ImageColor {
    uint8_t r, g, b, a;
};

class Image {
public:
    Image();
    void render();
private:
    ImageColor color;
    ImageShape shape;
};