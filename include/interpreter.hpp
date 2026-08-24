#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include "utils/AST.hpp"

#define SCRIBBLE_NUM_REP int32_t
#define SCRIBBLE_FLOAT_REP double

#define SymbolTableValues std::vector< std::pair< std::string, Interpreter::AnyValue > >

namespace Interpreter {
    enum EVAL_RES_TYPE : uint8_t {
        None,
        Num,
        Float,
        Bool,
        String,
        Object,

        RETURN, // for returning from a function without a value
    };

    struct AnyValue {
        std::vector<int>      dimension;
        std::shared_ptr<void> value;
        EVAL_RES_TYPE         type;
    };

    struct SharedSymbol {
        std::string               name;
        std::shared_ptr<AnyValue> value;
        std::mutex                symbol_mutex;
    };

    struct SymbolTable {
        std::shared_ptr<SymbolTable> parent;
        SymbolTableValues            values;
    };

    extern std::vector<SharedSymbol> GlobalValues;
    extern SymbolTableValues BuiltInVariables;

    extern std::shared_ptr<AST_Node> StartFunction;
    extern std::shared_ptr<AST_Node> UpdateFunction;
    extern std::unordered_map<std::string, std::shared_ptr<AST_Node>> Functions;
    extern std::unordered_map<std::string, EVAL_RES_TYPE> ValidDTypes;

    void InitRuntime();
    AnyValue eval(std::shared_ptr<AST_Node> root, std::shared_ptr<AnyValue> returnContext, std::shared_ptr<SymbolTable> memTable=nullptr);
}