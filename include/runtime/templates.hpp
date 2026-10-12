#pragma once

#include <string>
#include <vector>
#include "interpreter.hpp"
#include "graphics/images.hpp"

namespace TemplateObjects {
    class VariableWithProperties {
    public:
        virtual ~VariableWithProperties() = 0;
        virtual Interpreter::AnyValue CallFunction(std::string name, std::vector<Interpreter::AnyValue> args) = 0;
        virtual Interpreter::AnyValue GetValue(std::string name) = 0;
    };

    // Language level datatype, represents physical items on the screen
    class Object : public VariableWithProperties {
    public:
        Object();
        Object(Object* other); // for copies

        Interpreter::AnyValue CallFunction(std::string name, std::vector<Interpreter::AnyValue> args);
        Interpreter::AnyValue GetValue(std::string name);
    private:
        //SCRIBBLE_FLOAT_REP x, y;
        //Image img;
    };

    // User defined object (similar to "class" in C++)
    class Sprite : VariableWithProperties {
        
    };
}