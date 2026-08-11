#include "interpreter.hpp"
#include "debug.hpp"

using namespace Interpreter;

std::vector<SharedSymbol> GlobalValues;

AnyValue Interpreter::eval(AST_Node root, std::shared_ptr<SymbolTable> memTable) {
    switch(root.type) {
        case NODE_TYPE::PROGRAM:
            // run imports

            // load functions

            // 
            break;
        default:
            log("AST Node not recognized");
    }

    return AnyValue{};
}