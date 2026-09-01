#include <memory>
#include "runtime/built_in.hpp"

using namespace std;

void BuiltIn::init(SymbolTableValues &memory) {
    memory.clear();

    memory.emplace_back("JOY_X", Interpreter::AnyValue{{1}, make_shared<SCRIBBLE_FLOAT_REP>(0), Interpreter::EVAL_RES_TYPE::Float});
    memory.emplace_back("JOY_Y", Interpreter::AnyValue{{1}, make_shared<SCRIBBLE_FLOAT_REP>(0), Interpreter::EVAL_RES_TYPE::Float});
}

void BuiltIn::update(SymbolTableValues &memory) {

}