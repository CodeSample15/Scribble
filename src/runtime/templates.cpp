#include "runtime/templates.hpp"
#include "debug.hpp"

using namespace std;
using namespace TemplateObjects;
using namespace Interpreter;

TemplateObjects::VariableWithProperties::~VariableWithProperties() {};

TemplateObjects::Object::Object() {}
TemplateObjects::Object::Object(Object* other) {}

AnyValue TemplateObjects::Object::CallFunction(string name, vector<Interpreter::AnyValue> args) {
    log("FUNCTION CALLED: " + name);
    return AnyValue{};
}

AnyValue TemplateObjects::Object::GetValue(string name) {
    log("VALUE RETRIEVED: " + name);
    return AnyValue{};
}