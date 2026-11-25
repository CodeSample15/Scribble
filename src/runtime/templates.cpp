#include "runtime/templates.hpp"
#include "runtime/graphics.hpp"
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
        make_shared<SCRIBBLE_FLOAT_REP>(OBJECT_DEFAULT_WIDTH),
        EVAL_RES_TYPE::Float
    }});

    variables.push_back({"height", AnyValue{
        {1},
        make_shared<SCRIBBLE_FLOAT_REP>(OBJECT_DEFAULT_HEIGHT),
        EVAL_RES_TYPE::Float
    }});

    variables.push_back({"rotation", AnyValue{
        {1},
        make_shared<SCRIBBLE_FLOAT_REP>(0),
        EVAL_RES_TYPE::Float
    }});

    variables.push_back({"visible", AnyValue{
        {1},
        make_shared<bool>(true),
        EVAL_RES_TYPE::Bool
    }});

    variables.push_back({"color_r", AnyValue{
        {1},
        make_shared<SCRIBBLE_NUM_REP>(255),
        EVAL_RES_TYPE::Num
    }});

    variables.push_back({"color_g", AnyValue{
        {1},
        make_shared<SCRIBBLE_NUM_REP>(255),
        EVAL_RES_TYPE::Num
    }});

    variables.push_back({"color_b", AnyValue{
        {1},
        make_shared<SCRIBBLE_NUM_REP>(255),
        EVAL_RES_TYPE::Num
    }});

    variables.push_back({"color_a", AnyValue{
        {1},
        make_shared<SCRIBBLE_NUM_REP>(255),
        EVAL_RES_TYPE::Num
    }});

    auto defaultIcon = make_shared<sf::RectangleShape>();
    defaultIcon->setSize({OBJECT_DEFAULT_WIDTH, OBJECT_DEFAULT_HEIGHT});
    this->icon = defaultIcon;
    isShape = true;
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
    if(name == "draw") {
        Graphics::drawObj(this);
        return AnyValue{{}, nullptr, EVAL_RES_TYPE::None};
    } else if(name == "setColor") {
        if(args.size() < 3) throw ScribbleErr{0, 0, "setColor expected at least 3 args: (r, g, b, a=255)", ERR_TYPE::INVALID_FUN_CALL};

        return AnyValue{{}, nullptr, EVAL_RES_TYPE::None};   
    }

    throw ScribbleErr{0, 0, "Function name not found for Object: '" + name + "'", ERR_TYPE::INVALID_FUN_CALL};
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

void TemplateObjects::Object::setColor(int r, int g, int b, int a) {
    r = clamp(r, 0, 255);
    g = clamp(g, 0, 255);
    b = clamp(b, 0, 255);
    a = clamp(a, 0, 255);

    *(SCRIBBLE_NUM_REP*)GetValue("color_r").value.get() = r;
    *(SCRIBBLE_NUM_REP*)GetValue("color_g").value.get() = g;
    *(SCRIBBLE_NUM_REP*)GetValue("color_b").value.get() = b;
    *(SCRIBBLE_NUM_REP*)GetValue("color_a").value.get() = a;
}

void TemplateObjects::Object::updateIcon() {
    float x = *(SCRIBBLE_FLOAT_REP*)GetValue("x").value.get();
    float y = *(SCRIBBLE_FLOAT_REP*)GetValue("y").value.get();
    float width = *(SCRIBBLE_FLOAT_REP*)GetValue("width").value.get();
    float height = *(SCRIBBLE_FLOAT_REP*)GetValue("height").value.get();
    float rot = *(SCRIBBLE_FLOAT_REP*)GetValue("rotation").value.get();
    int r = *(SCRIBBLE_NUM_REP*)GetValue("color_r").value.get();
    int g = *(SCRIBBLE_NUM_REP*)GetValue("color_g").value.get();
    int b = *(SCRIBBLE_NUM_REP*)GetValue("color_b").value.get();
    int a = *(SCRIBBLE_NUM_REP*)GetValue("color_a").value.get();

    if(isShape) {
        auto iconShape = dynamic_cast<sf::RectangleShape*>(icon.get());
        iconShape->setPosition({x, y});
        iconShape->setSize({width, height});
        iconShape->setFillColor(sf::Color(r, g, b, a));
        iconShape->setRotation(sf::degrees(rot));
    } else {
        auto iconSprite = dynamic_cast<sf::Sprite*>(icon.get());
        iconSprite->setPosition({x, y});
        iconSprite->setScale({width, height});
        iconSprite->setColor(sf::Color(r, g, b, a));
        iconSprite->setRotation(sf::degrees(rot));
    }
}