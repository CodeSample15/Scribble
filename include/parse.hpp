#pragma once

#include <vector>
#include <functional>
#include <utility>

#include "utils/AST.hpp"
#include "lex.hpp"
#include "err.hpp"

//useful types
#define AST_Nib_Pair_t std::pair<Nibbler, AST_Node>
#define AST_Vec_Nib_Pair_t std::pair<Nibbler, std::vector<AST_Node>>
typedef std::vector<Token> tok_vec_t;
typedef std::vector<AST_Node> node_vec_t;

//helper class to consume tokens. Called nibbler cuz I'm taking small nibbles from the token vector (Similar to how the Rust library 'Nom' works)
class Nibbler {
public:
    Nibbler(tok_vec_t* tokens, size_t pos=0);
    Nibbler(size_t pos=0);
    Token next(); //throws ScribbleError when encountering an EOF
    size_t pos;
private:
    static tok_vec_t* tokens;
};

AST_Nib_Pair_t parse_program(Nibbler nibbler);

//preprocessor
AST_Nib_Pair_t parse_import_statement(Nibbler nibbler); //TODO

//main parse functions
AST_Nib_Pair_t parse_core_function(Nibbler nibbler);
AST_Nib_Pair_t parse_start_func(Nibbler nibbler);
AST_Nib_Pair_t parse_update_func(Nibbler nibbler);

//variable related
AST_Nib_Pair_t parse_vartype(Nibbler nibbler);
AST_Nib_Pair_t parse_variable_def(Nibbler nibbler);
AST_Nib_Pair_t parse_variable_reference(Nibbler nibbler);
AST_Nib_Pair_t parse_variable_assign(Nibbler nibbler);

//function related
AST_Nib_Pair_t parse_function_def(Nibbler nibbler);
AST_Nib_Pair_t parse_function_modifier(Nibbler nibbler);
AST_Nib_Pair_t parse_function_call(Nibbler nibbler);
AST_Nib_Pair_t parse_parameters(Nibbler nibbler);
AST_Nib_Pair_t parse_arguments(Nibbler nibbler);

AST_Nib_Pair_t parse_body(Nibbler nibbler);

//branch related
AST_Nib_Pair_t parse_branch(Nibbler nibbler);
AST_Nib_Pair_t parse_branch_if(Nibbler nibbler);
AST_Nib_Pair_t parse_branch_if_else(Nibbler nibbler);
AST_Nib_Pair_t parse_branch_else(Nibbler nibbler);

//loop related
AST_Nib_Pair_t parse_loop(Nibbler nibbler);
AST_Nib_Pair_t parse_while_loop(Nibbler nibbler);
AST_Nib_Pair_t parse_repeat_loop(Nibbler nibbler);

//expressions
AST_Nib_Pair_t parse_expression(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_orl(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_andl(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_or(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_xor(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_and(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_eq(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_cmp(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_shft(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_add(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_mult(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_pow(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_not(Nibbler nibbler);
AST_Nib_Pair_t parse_exp_primary(Nibbler nibbler);