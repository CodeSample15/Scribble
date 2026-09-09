#pragma once

#include "interpreter.hpp"

#define PRINT_FUNC_NAME "print"

namespace BuiltIn {
    void init(SymbolTableValues &memory, PreMadeFunctions &funcs);
    void update(SymbolTableValues &memory);
}