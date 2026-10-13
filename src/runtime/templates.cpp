#include "runtime/templates.hpp"
#include "debug.hpp"

using namespace std;
using namespace TemplateObjects;
using namespace Interpreter;

TemplateObjects::VariableWithProperties::~VariableWithProperties() {};

TemplateObjects::Object::Object() {
    variables.push_back({"x", AnyValue{
        {1},
        make_shared<SCRIBBLE_FLOAT_REP>(0),
        EVAL_RES_TYPE::Float
    }});

    variables.push_back({"y", AnyValue{
        {1},
        make_shared<SCRIBBLE_FLOAT_REP>(0),
        EVAL_RES_TYPE::Float
    }});

    variables.push_back({"width", AnyValue{
        {1},
        make_shared<SCRIBBLE_FLOAT_REP>(5),
        EVAL_RES_TYPE::Float
    }});

    variables.push_back({"height", AnyValue{
        {1},
        make_shared<SCRIBBLE_FLOAT_REP>(5),
        EVAL_RES_TYPE::Float
    }});

    variables.push_back({"visible", AnyValue{
        {1},
        make_shared<bool>(true),
        EVAL_RES_TYPE::Bool
    }});
}

TemplateObjects::Object::Object(Object* other) : Object() {
    // copy each value over from the other object
    for(auto &i : variables) {
        switch(i.second.type) {
            case EVAL_RES_TYPE::Num:
                *(SCRIBBLE_NUM_REP*)i.second.value.get() = *(SCRIBBLE_NUM_REP*) other->GetValue(i.first).value.get();
                break;
            case EVAL_RES_TYPE::Float:
                *(SCRIBBLE_FLOAT_REP*)i.second.value.get() = *(SCRIBBLE_FLOAT_REP*) other->GetValue(i.first).value.get();
                break;
            case EVAL_RES_TYPE::Bool:
                *(bool*)i.second.value.get() = *(bool*) other->GetValue(i.first).value.get();
                break;
            default:
                log("WARNING: Scribble developer screwed up, Object clone resulted in unexpected variable type being cloned");
                break;
        }
    }
}

AnyValue TemplateObjects::Object::CallFunction(string name, vector<Interpreter::AnyValue> args) {
    log("FUNCTION CALLED: " + name);
    return AnyValue{};
}

AnyValue TemplateObjects::Object::GetValue(string name) {
    for(auto &i : variables) {
        if(i.first == name)
            return i.second;
    }

    throw ScribbleErr{0, 0, "Object does not have member '" + name + "'", ERR_TYPE::INVALID_SYMBOL};
    return AnyValue{};
}