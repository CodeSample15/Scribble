#pragma once

#include "interpreter.hpp"

#define PRINT_FUNC_NAME "print"
#define SHAPE_FUNC_NAME "shape"
#define LEN_FUNC_NAME "len"

namespace BuiltIn {
    void init(SymbolTableValues &memory, PreMadeFunctions &funcs);
    void update(SymbolTableValues &memory);
}