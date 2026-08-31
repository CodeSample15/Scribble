#include <iostream>
#include "debug.hpp"

using namespace std;

void log(string s) {
    cout << s << endl;
}

void print_tokens(vector<Token> &tokens) 
{
    for(Token& tok : tokens) {
        cout << token_to_string(tok) << endl;
    }
}

string token_to_string(Token tok) {
    return "| " + tok_type_to_string(tok.type) + " | -- " + "line: " + to_string(tok.line) + " -- cols: (" + to_string(tok.start_col) + ", " + to_string(tok.end_col) + ") -- [ " + tok.lexeme + " ]";
}

string tok_type_to_string(TOK_TYPE tok) {
    switch (tok) {
        case USE: return "use";
        case AS: return "as";
        case IF: return "if";
        case ELSE_IF: return "else if";
        case ELSE: return "else";
        case WHILE: return "while";
        case REPEAT: return "repeat";
        case TRUE: return "true";
        case FALSE: return "false";
        case FUNCTION_DEFINE: return "function define";
        case NUMBER_TYPE: return "num type";
        case FLOAT_TYPE: return "float type";
        case BOOL_TYPE: return "Boolean type";
        case STRING_TYPE: return "string type";
        case IDENTIFIER: return "identifier";
        case STRING_LITERAL: return "string literal";
        case INT_LITERAL: return "int literal";
        case FLOAT_LITERAL: return "float literal";
        case OPEN_PAREN: return "open paren";
        case CLOSE_PAREN: return "close paren";
        case OPEN_CURLY: return "open curly";
        case CLOSE_CURLY: return "close curly";
        case OPEN_BRACKET: return "open bracket";
        case CLOSE_BRACKET: return "close bracket";
        case SEMICOLON: return "semicolon";
        case COMMA: return "comma";
        case PLUS: return "plus";
        case MINUS: return "minus";
        case SLASH: return "slash";
        case STAR: return "star";
        case BAR: return "bar";
        case UP_ARROW: return "up arrow";
        case GREATER_THAN: return "greater than";
        case LESS_THAN: return "less than";
        case GREATER_THAN_EQUAL: return "greater than / equal to";
        case LESS_THAN_EQUAL: return "less than / equal to";
        case SHIFT_RIGHT: return "shift right";
        case SHIFT_LEFT: return "shift left";
        case POW: return "power";
        case EQUALS: return "equals";
        case PLUS_EQUALS: return "plus equals";
        case MINUS_EQUALS: return "minus equals";
        case SLASH_EQUALS: return "slash equals";
        case STAR_EQUALS: return "star equals";
        case IMAGE_REF: return "image ref";
        case BUILT_IN_VARIABLE_REF: return "built in variable ref";
        case SPECIAL_FUNCTION_PREFIX: return "special function prefix";
        case AND: return "and";
        case OR: return "or";
        case NOT: return "not";
        case CMP_EQUALS: return "equal to";
        case CMP_NOT_EQUALS: return "not equal";
        case WHITESPACE: return "whitespace";
        case COMMENT: return "comment";
        case OTHER: return "other";
        default: return "TOK NOT FOUND";
    }
}

std::string AST_node_type_to_string(NODE_TYPE node) {
    switch(node) {
        case PROGRAM: return "program";
        case IMPORT_STATEMENT: return "import statement";
        case CORE_FUNCTION: return "core function";
        case FUNCTION_DEF: return "function def";
        case VARIABLE_REFERENCE: return "variable reference";
        case BUILT_IN_VAR_REFERENCE: return "built in variable";
        case NORMAL_VAR_REFERENCE: return "normal variable";
        case VARIABLE_DEF: return "variable def";
        case VARIABLE_ASSIGN: return "variable assign";
        case ARR_INDEX: return "array index";
        case START_FUNC: return "start func";
        case UPDATE_FUNC: return "update func";
        case VAR_TYPE: return "var type";
        case ASSIGN_OP: return "assign operator";
        case BODY: return "body";
        case BRANCH: return "branch";
        case BRANCH_IF: return "if";
        case BRANCH_ELSE_IF: return "else if";
        case BRANCH_ELSE: return "else";
        case LOOP_WHILE: return "loop while";
        case LOOP_REPEAT: return "loop repeat";
        case FUNCTION_MODIFIER: return "function modifier";
        case FUNCTION_CALL: return "function call";
        case ARGUMENTS: return "arguments";
        case PARAMETERS: return "parameters";
        case IDENT: return "identifier";
        case EXP_ORL: return "logical OR";
        case EXP_ANDL: return "logical AND";
        case EXP_OR: return "bitwise OR";
        case EXP_XOR: return "bitwise XOR";
        case EXP_AND: return "bitwise AND";
        case EXP_EQ: return "equal/not equal";
        case EXP_CMP: return "comparison";
        case EXP_SHFT: return "shift operator";
        case EXP_ADD: return "add/sub";
        case EXP_MULT: return "multiplication op";
        case EXP_POW: return "power";
        case EXP_NOT: return "not";
        case EXP_PRIMARY: return "primary eq";
        case NON: return "NON";

        default: return "WARNING: NO TYPE TO STRING";
    }
}

std::string data_type_to_string(Interpreter::EVAL_RES_TYPE type) {
    switch(type) {
        case Interpreter::Num: return "num";
        case Interpreter::Float: return "float";
        case Interpreter::Bool: return "bool";
        case Interpreter::String: return "string";
        case Interpreter::Object: return "Object";

        default: return "Other";
    }
}

void print_AST(AST_Node &root) {
    cout << AST_To_String(root, true, 0) << endl;
}

//helper function for printing AST
static string get_tab_space(int tab);

string AST_To_String(AST_Node &root, bool recursive, int layer) {
    string res = get_tab_space(layer) + AST_node_type_to_string(root.type) + ": \n";
    
    if(root.tok != nullptr && root.tok->type != TOK_TYPE::OTHER)
        res += get_tab_space(layer+1) + token_to_string(*root.tok) + "\n";
    
    if(recursive && root.children.size() != 0) {
        res += get_tab_space(layer+1) + "Children:\n";
        for(auto& i : root.children)
            res += AST_To_String(*i, true, layer+2) + "\n";
    }

    return res;
}

string get_tab_space(int tab) {
    string res = "";
    for(int i=0; i<tab; i++) {
        res += "   ";
    }
    return res;
}