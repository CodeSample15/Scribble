#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

#include "interpreter.hpp"

#define OBJECT_DEFAULT_WIDTH 50
#define OBJECT_DEFAULT_HEIGHT 50

namespace TemplateObjects {
    class VariableWithProperties {
    public:
        virtual ~VariableWithProperties() = 0;
        virtual Interpreter::AnyValue CallFunction(std::string name, std::vector<Interpreter::AnyValue> args) = 0;
        virtual Interpreter::AnyValue GetValue(std::string name) = 0;
    };

    enum ObjectShape {
        CIRCLE,
        SQUARE,
        TRIANGLE,
        PENTAGON,
        HEXAGON,
        OCTAGON,
        CUSTOM
    };

    // Language level datatype, represents physical items on the screen
    class Object : public VariableWithProperties {
    public:
        Object();
        Object(Object* other); // for copies

        Interpreter::AnyValue CallFunction(std::string name, std::vector<Interpreter::AnyValue> args);
        Interpreter::AnyValue GetValue(std::string name);

        std::shared_ptr<sf::Drawable> icon;
    private:
        SymbolTableValues variables;
        //ObjectShape shape;
        // std::shared_ptr<sf::Drawable> icon;
    };

    // User defined object (similar to "class" in C++)
    class Prefab : VariableWithProperties {
        
    };
}