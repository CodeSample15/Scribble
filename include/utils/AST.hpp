#pragma once

#include <vector>
#include <memory>
#include "lex.hpp"

enum NODE_TYPE : uint8_t {
    PROGRAM,
    IMPORT_STATEMENT, VARIABLE_DEF, VARIABLE_ASSIGN,
    VARIABLE_REFERENCE, BUILT_IN_VAR_REFERENCE, NORMAL_VAR_REFERENCE,
    ARR_INDEX,
    CORE_FUNCTION, START_FUNC, UPDATE_FUNC,

    VAR_TYPE,
    ASSIGN_OP,

    BODY, BRANCH,

    BRANCH_IF, BRANCH_ELSE_IF, BRANCH_ELSE,

    LOOP_WHILE, LOOP_REPEAT,

    FUNCTION_DEF, FUNCTION_MODIFIER, FUNCTION_CALL, ARGUMENTS, PARAMETERS,
    RETURN_STATEMENT,

    IDENT,

    EXP_ORL, EXP_ANDL, EXP_OR, EXP_XOR, EXP_AND, EXP_EQ, EXP_CMP,
    EXP_SHFT, EXP_ADD, EXP_MULT, EXP_POW, EXP_NOT, EXP_PRIMARY,

    NON
};

struct AST_Node {
    std::shared_ptr<Token> tok;
    std::vector<std::shared_ptr< AST_Node >> children;
    NODE_TYPE type;

    AST_Node() : tok(nullptr) {
        this->type=NON;
    }
    AST_Node(NODE_TYPE type) : tok(nullptr) {
        this->type = type;
    }
};

