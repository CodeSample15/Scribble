#include "parse.hpp"
#include "debug.hpp"

#define NON_NODE AST_Node(NON)

using namespace std;

//helper function prototypes
AST_Nib_Pair_t alt_types(Nibbler nibbler, std::vector<TOK_TYPE> types);
AST_Nib_Pair_t alt(Nibbler nibbler, std::vector<std::function< AST_Nib_Pair_t(Nibbler) >> funcs);
AST_Nib_Pair_t require(Nibbler nibbler, TOK_TYPE type);
AST_Nib_Pair_t opt(Nibbler nibbler, TOK_TYPE type);
AST_Nib_Pair_t opt(Nibbler nibbler, std::function< Nibbler(Nibbler) > func);
AST_Vec_Nib_Pair_t many_0(Nibbler nibbler, std::function< AST_Nib_Pair_t(Nibbler) > func);
Nibbler many_0_lambda(Nibbler nibbler, std::function< Nibbler(Nibbler) > func);

AST_Nib_Pair_t expression_seg_parse(Nibbler nibbler, std::function< AST_Nib_Pair_t(Nibbler) > expression_seg, vector<TOK_TYPE> exp_symbols, NODE_TYPE out_type);

void push_children(AST_Node &parent, vector<AST_Node> children, bool ignoreNon=true);

//{import_statement} , {core_function | function_def | variable_def}
AST_Nib_Pair_t parse_program(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}


//PREPROCESSOR

//TODO

//MAIN STUFF

//start_func | update_func
AST_Nib_Pair_t parse_core_function(Nibbler nibbler) {
    return alt(nibbler, {parse_start_func, parse_update_func});
}

//':START:{' , body , '}'
AST_Nib_Pair_t parse_start_func(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

//':UPDATE:{' , body , '}'
AST_Nib_Pair_t parse_update_func(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

//parser functions

AST_Nib_Pair_t parse_vartype(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

// VARTYPE , identifier , {',' , identifier} , ['=' , expression]
AST_Nib_Pair_t parse_variable_def(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

//['$'] , identifier , ['[' , arguments , ']']
AST_Nib_Pair_t parse_variable_reference(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

//variable_reference , INCR_DECR_OP | (ASSIGN_OP , expression)
AST_Nib_Pair_t parse_variable_assign(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

//FUNCTIONS

//[function_modifier] , 'fun' , identifier , '(' , [parameters] , ')' , '{' , body , '}'
AST_Nib_Pair_t parse_function_def(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

//'[' , VALID_FUNCTION_MODIFIER , {',' , VALID_FUNCTION_MODIFIER} , ']'
AST_Nib_Pair_t parse_function_modifier(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

//identifier , '(' , [arguments] , ')'
AST_Nib_Pair_t parse_function_call(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

//VARTYPE , identifier , {',' , VARTYPE , identifier}
AST_Nib_Pair_t parse_parameters(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

//expression , { ',' , expression }
AST_Nib_Pair_t parse_arguments(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

// { (variable_def | variable_assign | branch | function_call | loop) , [';'] }
AST_Nib_Pair_t parse_body(Nibbler nibbler) {
    node_vec_t body_children;

    tie(nibbler, body_children) = many_0(nibbler, [&](Nibbler n) {
        AST_Node res;

        tie(nibbler, res) = alt(nibbler, {parse_variable_def, parse_variable_assign, parse_branch, parse_function_call, parse_loop});
        nibbler = opt(nibbler, TOK_TYPE::SEMICOLON).first;

        return (AST_Nib_Pair_t){n, res};
    });
    
    AST_Node body(NODE_TYPE::BODY);
    push_children(body, body_children);
    
    return {nibbler, body};
}

//BRANCHES

// branch = branch_if ,  {branch_ifelse} , [branch_else]
AST_Nib_Pair_t parse_branch(Nibbler nibbler) {
    AST_Node ifNode, elseNode;
    node_vec_t ifElseNodes;

    tie(nibbler, ifNode) = parse_branch_if(nibbler);
    tie(nibbler, ifElseNodes) = many_0(nibbler, parse_branch_if_else);
    tie(nibbler, elseNode) = opt(nibbler, parse_branch_else);

    AST_Node res(NODE_TYPE::BRANCH);
    push_children(res, {ifNode});
    push_children(res, ifElseNodes);
    push_children(res, {elseNode});

    return {nibbler, res};
}

// 'if' , expression , '{' , body , '}'
AST_Nib_Pair_t parse_branch_if(Nibbler nibbler) {
    AST_Node expression, body;

    nibbler = require(nibbler, TOK_TYPE::IF).first;
    tie(nibbler, expression) = parse_expression(nibbler);
    nibbler = require(nibbler, TOK_TYPE::OPEN_CURLY).first;
    tie(nibbler, body) = parse_body(nibbler);
    nibbler = require(nibbler, TOK_TYPE::CLOSE_CURLY).first;

    AST_Node res(NODE_TYPE::BRANCH_IF);
    push_children(res, {expression, body});

    return {nibbler, res};
}

// 'if else' , expression , '{' , body , '}'
AST_Nib_Pair_t parse_branch_if_else(Nibbler nibbler) {
    AST_Node expression, body;

    nibbler = require(nibbler, TOK_TYPE::IF_ELSE).first;
    tie(nibbler, expression) = parse_expression(nibbler);
    nibbler = require(nibbler, TOK_TYPE::OPEN_CURLY).first;
    tie(nibbler, body) = parse_body(nibbler);
    nibbler = require(nibbler, TOK_TYPE::CLOSE_CURLY).first;

    AST_Node res(NODE_TYPE::BRANCH_IF_ELSE);
    push_children(res, {expression, body});

    return {nibbler, res};
}

// 'else' , '{' , body , '}'
AST_Nib_Pair_t parse_branch_else(Nibbler nibbler) {
    AST_Node body;

    nibbler = require(nibbler, TOK_TYPE::ELSE).first;
    nibbler = require(nibbler, TOK_TYPE::OPEN_CURLY).first;
    tie(nibbler, body) = parse_body(nibbler);
    nibbler = require(nibbler, TOK_TYPE::CLOSE_CURLY).first;

    AST_Node res(NODE_TYPE::LOOP_REPEAT);
    push_children(res, {body});

    return {nibbler, res};
}

//LOOPS

// while_loop | repeat_loop
AST_Nib_Pair_t parse_loop(Nibbler nibbler) {
    return alt(nibbler, {parse_while_loop, parse_repeat_loop});
}

// 'while' , expression , '{' , body , '}'
AST_Nib_Pair_t parse_while_loop(Nibbler nibbler) {
    AST_Node expression, body;

    nibbler = require(nibbler, TOK_TYPE::WHILE).first;
    tie(nibbler, expression) = parse_expression(nibbler);
    nibbler = require(nibbler, TOK_TYPE::OPEN_CURLY).first;
    tie(nibbler, body) = parse_body(nibbler);
    nibbler = require(nibbler, TOK_TYPE::CLOSE_CURLY).first;

    AST_Node res(NODE_TYPE::LOOP_WHILE);
    push_children(res, {expression, body});

    return {nibbler, res};
}

// 'repeat' , expression , '{' , body , '}'
AST_Nib_Pair_t parse_repeat_loop(Nibbler nibbler) {
    AST_Node expression, body;

    nibbler = require(nibbler, TOK_TYPE::REPEAT).first;
    tie(nibbler, expression) = parse_expression(nibbler);
    nibbler = require(nibbler, TOK_TYPE::OPEN_CURLY).first;
    tie(nibbler, body) = parse_body(nibbler);
    nibbler = require(nibbler, TOK_TYPE::CLOSE_CURLY).first;

    AST_Node res(NODE_TYPE::LOOP_REPEAT);
    push_children(res, {expression, body});

    return {nibbler, res};
}

//EXPRESSIONS

//exp_andl , { '||' , exp_andl }
AST_Nib_Pair_t parse_expression(Nibbler nibbler) {
    return expression_seg_parse(nibbler, parse_exp_andl, {TOK_TYPE::OR}, NODE_TYPE::EXP_ORL);
}

//exp_or , { '&&' , exp_or }
AST_Nib_Pair_t parse_exp_andl(Nibbler nibbler) {
    return expression_seg_parse(nibbler, parse_exp_or, {TOK_TYPE::AND}, NODE_TYPE::EXP_ANDL);
}

//exp_xor , { '|' , exp_xor }
AST_Nib_Pair_t parse_exp_or(Nibbler nibbler) {
    return expression_seg_parse(nibbler, parse_exp_xor, {TOK_TYPE::BAR}, NODE_TYPE::EXP_OR);
}

//exp_and , { '^' , exp_and }
AST_Nib_Pair_t parse_exp_xor(Nibbler nibbler) {
    return expression_seg_parse(nibbler, parse_exp_and, {TOK_TYPE::UP_ARROW}, NODE_TYPE::EXP_XOR);
}

//exp_eq , { '&' , exp_eq }
AST_Nib_Pair_t parse_exp_and(Nibbler nibbler) {
    return expression_seg_parse(nibbler, parse_exp_eq, {TOK_TYPE::BIT_AND}, NODE_TYPE::EXP_AND);
}

//exp_cmp , { ('==' | '!=') , exp_cmp }
AST_Nib_Pair_t parse_exp_eq(Nibbler nibbler) {
    return expression_seg_parse(nibbler, parse_exp_cmp, {TOK_TYPE::CMP_EQUALS, TOK_TYPE::CMP_NOT_EQUALS}, NODE_TYPE::EXP_EQ);
}

//exp_shft , { ('<' | '<=' | '>' | '>=') , exp_shft }
AST_Nib_Pair_t parse_exp_cmp(Nibbler nibbler) {
    return expression_seg_parse(nibbler, parse_exp_shft, {TOK_TYPE::LESS_THAN, TOK_TYPE::LESS_THAN_EQUAL, TOK_TYPE::GREATER_THAN, TOK_TYPE::GREATER_THAN_EQUAL}, NODE_TYPE::EXP_CMP);
}

//exp_add , { ('<<' , '>>') , exp_add }
AST_Nib_Pair_t parse_exp_shft(Nibbler nibbler) {
    return expression_seg_parse(nibbler, parse_exp_add, {TOK_TYPE::SHIFT_LEFT, TOK_TYPE::SHIFT_RIGHT}, NODE_TYPE::EXP_SHFT);
}

//exp_mult , { ('+' | '-') , exp_mult }
AST_Nib_Pair_t parse_exp_add(Nibbler nibbler) {
    return expression_seg_parse(nibbler, parse_exp_mult, {TOK_TYPE::PLUS, TOK_TYPE::MINUS}, NODE_TYPE::EXP_ADD);
}

//exp_pow , { ('*' | '/' | '%') , exp_pow }
AST_Nib_Pair_t parse_exp_mult(Nibbler nibbler) {
    return expression_seg_parse(nibbler, parse_exp_pow, {TOK_TYPE::STAR, TOK_TYPE::SLASH, TOK_TYPE::PERCENT}, NODE_TYPE::EXP_MULT);
}

//exp_not , [ '**' , exp_pow ]
AST_Nib_Pair_t parse_exp_pow(Nibbler nibbler) {
    return {nibbler, NON_NODE};
}

//['!'] , exp_primary
AST_Nib_Pair_t parse_exp_not(Nibbler nibbler) {
    AST_Node not_node;
    bool use_not = false;
    try {
        tie(nibbler, not_node) = require(nibbler, TOK_TYPE::NOT);
        use_not = true;
    } catch(ScribbleErr&) {}

    AST_Node primary;
    tie(nibbler, primary) = parse_exp_primary(nibbler);

    if(use_not) {
        not_node.children.push_back(primary);
        return {nibbler, not_node};
    }

    return {nibbler, primary};
}

//variable_reference | literal | '(' , expression , ')' | function_call
AST_Nib_Pair_t parse_exp_primary(Nibbler nibbler) {
    return alt(nibbler, {parse_variable_reference, 
                parse_function_call,
                [&](Nibbler n){ return alt_types(nibbler, {TOK_TYPE::STRING_LITERAL, TOK_TYPE::INT_LITERAL, TOK_TYPE::FLOAT_LITERAL, TOK_TYPE::TRUE, TOK_TYPE::FALSE}); },
                [&](Nibbler n){
                    AST_Node expr;
                    n = require(n, TOK_TYPE::OPEN_PAREN).first;
                    tie(n, expr) = parse_expression(n);
                    n = require(n, TOK_TYPE::CLOSE_PAREN).first;

                    return (AST_Nib_Pair_t){n, expr};
                }});
}

//helper function 
AST_Nib_Pair_t expression_seg_parse(Nibbler nibbler, std::function< AST_Nib_Pair_t(Nibbler) > expression_seg, vector<TOK_TYPE> exp_symbols, NODE_TYPE out_type) {
    AST_Node first;
    tie(nibbler, first) = expression_seg(nibbler);

    nibbler = many_0_lambda(nibbler, [&](Nibbler n) {
        AST_Node symbol;
        tie(n, symbol) = alt_types(n, exp_symbols);

        AST_Node second;
        tie(n, second) = expression_seg(n);
        
        symbol.children.push_back(first);
        symbol.children.push_back(second);
        symbol.type = out_type;
        first = symbol; //return this symbol to join the two segments

        return n;
    });

    return {nibbler, first};
}

//define helper functions
AST_Nib_Pair_t alt_types(Nibbler nibbler, std::vector<TOK_TYPE> types) {
    ScribbleErr last_e = {0, 0, "", ERR_TYPE::EXPECTED};

    for(TOK_TYPE &type : types) {
        try {
            return require(nibbler, type);
        } catch(ScribbleErr &e) { last_e = e; }
    }

    throw last_e;
    return {nibbler, NON_NODE}; //this line only exists to please the compiler god
}

AST_Nib_Pair_t alt(Nibbler nibbler, std::vector<std::function< AST_Nib_Pair_t(Nibbler) >> funcs) {
    ScribbleErr last_e = {0, 0, "", ERR_TYPE::EXPECTED};

    //loop through each function to try to find a pattern match
    for(auto &func : funcs) {
        try {
            return func(nibbler); //if this returns without throwing an error a pattern was found
        } catch(ScribbleErr &e) { last_e = e; }
    }
    
    //if no pattern was found throw the last error reaped from scanning
    throw last_e;
    return {nibbler, NON_NODE}; //this line only exists to please the compiler god
}

AST_Nib_Pair_t require(Nibbler nibbler, TOK_TYPE type) {
    AST_Node tmp(NODE_TYPE::NON);
    Token next = nibbler.next();

    if(next.type == type) {
        //set tmp node type depending on the token type
        switch(type) {
            case TOK_TYPE::STRING_LITERAL:
            case TOK_TYPE::INT_LITERAL:
            case TOK_TYPE::FLOAT_LITERAL:
            tmp.type = NODE_TYPE::EXP_PRIMARY;
            break;

            default: break;
        }

        tmp.tok = std::make_unique<Token>(next);
        return {nibbler, tmp};
    }

    throw (ScribbleErr) { next.line, next.start_col, tok_type_to_string(type), ERR_TYPE::EXPECTED };
    return {nibbler, NON_NODE};
}

AST_Nib_Pair_t opt(Nibbler nibbler, TOK_TYPE type) {
    try {
        return require(nibbler, type); //return new location
    } catch(ScribbleErr&) {}

    return {nibbler, NON_NODE}; //return original location
}

AST_Nib_Pair_t opt(Nibbler nibbler, std::function< AST_Nib_Pair_t(Nibbler) > func) {
    try {
        return func(nibbler); //return new location
    } catch(ScribbleErr&) {}

    return {nibbler, NON_NODE}; //return original location
}

AST_Vec_Nib_Pair_t many_0(Nibbler nibbler, std::function< AST_Nib_Pair_t(Nibbler) > func) {
    vector<AST_Node> res;

    while(true) {
        try {
            AST_Node tmp;
            tie(nibbler, tmp) = func(nibbler);
            res.push_back(tmp);
        } catch(ScribbleErr&) { break; }
    }

    return {nibbler, res};
}

Nibbler many_0_lambda(Nibbler nibbler, std::function< Nibbler(Nibbler) > func) {
    while(true) {
        try {
            nibbler = func(nibbler);
        } catch(ScribbleErr&) { break; }
    }

    return nibbler;
}

void push_children(AST_Node &parent, vector<AST_Node> children, bool ignoreNon) {
    for(AST_Node &c : children) {
        if(ignoreNon && c.type == NODE_TYPE::NON) continue;
        parent.children.push_back(c);
    }
}

//define the Nibbler helper class
tok_vec_t* Nibbler::tokens = nullptr;

Nibbler::Nibbler(tok_vec_t* tokens, size_t pos) {
    Nibbler::tokens = tokens;
    this->pos = pos;
}

Nibbler::Nibbler(size_t pos) {
    this->pos = pos;
}

Token Nibbler::next() {
    if(pos >= tokens->size()) throw (ScribbleErr) {0, 0, "", ERR_TYPE::EOT};
    return tokens->at(pos++);
}