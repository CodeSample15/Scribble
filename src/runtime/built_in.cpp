#include <memory>
#include <chrono>

#include "runtime/built_in.hpp"
#include "runtime/graphics.hpp"
#include "err.hpp"
#include "debug.hpp"

using namespace std;
using namespace Interpreter;

// built in functions prototypes
AnyValue BI_fun_print(vector<AnyValue>);
AnyValue BI_fun_shape(vector<AnyValue>);
AnyValue BI_fun_len(vector<AnyValue>);

AnyValue noValue();

static chrono::steady_clock::time_point LastFrameTime;
static bool RanFirstFrame = false;

// helper prototypes
void assert_arg_types(string funName, vector<EVAL_RES_TYPE> types, vector<AnyValue> &args);

void BuiltIn::init(SymbolTableValues &memory, PreMadeFunctions &funcs) {
    memory.clear();
    memory.emplace_back("JOY_X", AnyValue{{1}, make_shared<SCRIBBLE_FLOAT_REP>(0), EVAL_RES_TYPE::Float});
    memory.emplace_back("JOY_Y", AnyValue{{1}, make_shared<SCRIBBLE_FLOAT_REP>(0), EVAL_RES_TYPE::Float});

    memory.emplace_back("BTN_A", AnyValue{{1}, make_shared<bool>(false), EVAL_RES_TYPE::Bool});
    memory.emplace_back("BTN_B", AnyValue{{1}, make_shared<bool>(false), EVAL_RES_TYPE::Bool});
    memory.emplace_back("BTN_C", AnyValue{{1}, make_shared<bool>(false), EVAL_RES_TYPE::Bool});
    memory.emplace_back("BTN_ESC", AnyValue{{1}, make_shared<bool>(false), EVAL_RES_TYPE::Bool});

    memory.emplace_back("SCREEN_WIDTH", AnyValue{{1}, make_shared<SCRIBBLE_NUM_REP>(0), EVAL_RES_TYPE::Num});
    memory.emplace_back("SCREEN_HEIGHT", AnyValue{{1}, make_shared<SCRIBBLE_NUM_REP>(0), EVAL_RES_TYPE::Num});
    memory.emplace_back("DTIME", AnyValue{{1}, make_shared<SCRIBBLE_FLOAT_REP>(0), EVAL_RES_TYPE::Float});

    funcs.clear();
    funcs.insert({PRINT_FUNC_NAME, BI_fun_print});
    funcs.insert({SHAPE_FUNC_NAME, BI_fun_shape});
    funcs.insert({LEN_FUNC_NAME, BI_fun_len});
}

void BuiltIn::update(SymbolTableValues &memory) {
    for(auto &p : memory) {
        if (p.first == "JOY_X") {
            // TODO: use SFML Joystick to make Scribble work with joysticks
            *(SCRIBBLE_FLOAT_REP*)p.second.value.get() = (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right) ? 1 : 0) + (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left) ? -1 : 0);
        } else if (p.first == "JOY_Y") {
            *(SCRIBBLE_FLOAT_REP*)p.second.value.get() = (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up) ? 1 : 0) + (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down) ? -1 : 0);
        } else if (p.first == "BTN_A") {
            *(bool*)p.second.value.get() = sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Z);
        } else if (p.first == "BTN_B") {
            *(bool*)p.second.value.get() = sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::X);
        } else if (p.first == "BTN_C") {
            *(bool*)p.second.value.get() = sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::C);
        } else if (p.first == "BTN_ESC") {
            *(bool*)p.second.value.get() = sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Escape);
        } else if (p.first == "SCREEN_WIDTH") {
            *(SCRIBBLE_NUM_REP*)p.second.value.get() = Graphics::ScreenWidth;
        } else if (p.first == "SCREEN_HEIGHT") {
            *(SCRIBBLE_NUM_REP*)p.second.value.get() = Graphics::ScreenHeight;
        } else if (p.first == "DTIME") {
            auto now = chrono::steady_clock::now();
            *(SCRIBBLE_FLOAT_REP*)p.second.value.get() = RanFirstFrame ? 
                                                        (float)chrono::duration_cast<chrono::nanoseconds>(now - LastFrameTime).count() / 10000000
                                                        : 0;
            LastFrameTime = now;
            RanFirstFrame = true;
        }
    }
}

// define built in functions
AnyValue BI_fun_print(vector<AnyValue> args) {
    assert_arg_types(PRINT_FUNC_NAME, {EVAL_RES_TYPE::String}, args);
    log(*(string*)args[0].value.get());
    return noValue();
}

AnyValue BI_fun_shape(vector<AnyValue> args) {
    assert_arg_types(LEN_FUNC_NAME, {EVAL_RES_TYPE::None}, args); // None type expected because arg can be of any type
    if(args[0].dimension.size() == 0)
        throw ScribbleErr{0,0, "Function got invalid parameter", ERR_TYPE::INVALID_FUN_CALL};

    shared_ptr<vector<AnyValue>> res = make_shared<vector<AnyValue>>();
    for(int i : args[0].dimension) {
        res->push_back(AnyValue{
            {1},
            make_shared<SCRIBBLE_NUM_REP>(i),
            EVAL_RES_TYPE::Num
        });
    }

    return AnyValue {
        {(int)res->size()},
        res,
        EVAL_RES_TYPE::Num
    };
}

AnyValue BI_fun_len(vector<AnyValue> args) {
    assert_arg_types(LEN_FUNC_NAME, {EVAL_RES_TYPE::None}, args); // None type expected because arg can be of any type
    if(args[0].dimension.size() == 0)
        throw ScribbleErr{0,0, "Function got invalid parameter", ERR_TYPE::INVALID_FUN_CALL};

    SCRIBBLE_NUM_REP res = args[0].dimension[0];

    return AnyValue {
        {1},
        make_shared<SCRIBBLE_NUM_REP>(res),
        EVAL_RES_TYPE::Num
    };
}

// define helper functions
void assert_arg_types(string funName, vector<EVAL_RES_TYPE> types, vector<AnyValue> &args) {
    if(types.size() != args.size())
        // position of where error occurred is appended by calling function in interpreter, for now line and column is just zero
        throw ScribbleErr{0,0, "Built in function '" + funName + "' expected " + to_string(types.size()) + " arguments, got " + to_string(args.size()), ERR_TYPE::INVALID_FUN_CALL};

    for(size_t i=0; i<types.size(); i++) {
        if(types[i] == EVAL_RES_TYPE::None) continue;

        if(args[i].type != types[i]) {
            // construct a list of what args were actually expected
            string expectedTypes = "";
            for(size_t j=0; j<types.size(); j++) {
                expectedTypes += data_type_to_string(types[j]);
                if(j+1 < types.size())
                    expectedTypes += ", ";
            }

            // position of where error occurred is appended by calling function in interpreter, for now line and column is just zero
            throw ScribbleErr{0,0, "Invalid call to '" + funName + "': Expected arguments of types <" + expectedTypes + ">", ERR_TYPE::INVALID_FUN_CALL};
        }
    }
}

AnyValue noValue() {
    AnyValue t;
    t.type = EVAL_RES_TYPE::None;
    return t;
}