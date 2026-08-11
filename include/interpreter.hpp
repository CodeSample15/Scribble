#pragma once

#include <memory>
#include <mutex>
#include <string>
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
        std::mutex                  mut;
    };

    struct SymbolTable {
        std::shared_ptr<SymbolTable>    parent;
        SymbolTableValues               values;
    };

    extern std::vector<SharedSymbol> GlobalValues;
    AnyValue eval(AST_Node root, std::shared_ptr<SymbolTable> memTable=nullptr);
}