#pragma once

#include "lex.hpp"
#include "parse.hpp"
#include "interpreter.hpp"

/*
    Functions which will be used to debug the Scribble interpreter
*/

void log(std::string s);

std::string tok_type_to_string(TOK_TYPE tok);
std::string AST_node_type_to_string(NODE_TYPE node);
std::string data_type_to_string(Interpreter::EVAL_RES_TYPE type);

void print_tokens(std::vector<Token> &tokens);
std::string token_to_string(Token tok);

void print_AST(AST_Node &root);
std::string AST_To_String(AST_Node &root, bool recursive, int layer);