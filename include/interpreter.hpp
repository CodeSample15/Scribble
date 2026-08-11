#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include "utils/AST.hpp"

#define SymbolTableValues std::vector< std::pair< std::string, std::shared_ptr< Interpreter::AnyValue > > >

namespace Interpreter {
    enum EvalResultType {
        None,
        Num,
        Float,
        String,
        Object
    };

    struct AnyValue {
        EvalResultType          type;
        std::vector<int>        dimension;

        std::shared_ptr<void>   value;
    };

    struct SharedSymbol {
        std::string                 name;
        std::shared_ptr<AnyValue>   value;
        std::mutex                  symbol_mutex;
    };

    struct SymbolTable {
        std::shared_ptr<SymbolTable>    parent;
        SymbolTableValues               values;
    };

    extern std::vector<SharedSymbol> GlobalValues;

    extern std::shared_ptr<AST_Node> StartFunction;
    extern std::shared_ptr<AST_Node> UpdateFunction;
    extern std::unordered_map<std::string, std::shared_ptr<AST_Node>> functions;

    AnyValue eval(AST_Node root, std::shared_ptr<SymbolTable> memTable=nullptr);
}