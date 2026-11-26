#pragma once

#include "interpreter.hpp"
#include "err.hpp"

namespace Utils {
    // Interpreter helper functions:
    void checkForAllowedDtype(Interpreter::AnyValue val, std::vector<Interpreter::EVAL_RES_TYPE> dtypes, std::shared_ptr<AST_Node> &node);
    bool isPrimitive(Interpreter::EVAL_RES_TYPE t); // returns true if the passed type is a primitive type
    void checkSingleVal(Interpreter::AnyValue val, std::shared_ptr<AST_Node> &node);
    std::shared_ptr<void> defaultValueFor(Interpreter::EVAL_RES_TYPE);
    std::shared_ptr<void> duplicateValue(Interpreter::EVAL_RES_TYPE, std::shared_ptr<void>);
    std::shared_ptr<void> newArrayOfShape(std::vector<int> shape, Interpreter::EVAL_RES_TYPE dtype);
    Interpreter::AnyValue valueFromArrayIndex(Interpreter::AnyValue arr, std::vector<int> index);
    std::pair<std::shared_ptr<void>, Interpreter::EVAL_RES_TYPE> castNumValue(double val, Interpreter::EVAL_RES_TYPE type1, Interpreter::EVAL_RES_TYPE type2);
    double extractNumValue(Interpreter::AnyValue &val, std::shared_ptr<AST_Node> &node);
    void castAndAssign(Interpreter::AnyValue &val, double newVal, bool inPlace=false);
    void indexVariable(Interpreter::AnyValue* val, AST_Node* idx, std::shared_ptr<std::vector<size_t>> ownedGlobalVariables, std::shared_ptr<Interpreter::SymbolTable> memTable);

    void throwScribbleError(std::shared_ptr<AST_Node> node, std::string message, ERR_TYPE type, std::shared_ptr<ScribbleErr> parent=nullptr);
}