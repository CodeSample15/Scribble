#pragma once

#include <vector>
#include <string>

enum TOK_TYPE {
    //preprocessor
    USE,                            // use
    AS,                             // as

    //keywords
    IF,                             // if
    IF_ELSE,                        // if else
    ELSE,                           // else

    WHILE,                          // while
    REPEAT,                         // repeat

    TRUE,                           //true
    FALSE,                          //false

    FUNCTION_DEFINE,                // fun

    //built in types
    NUMBER_TYPE,                    // num
    FLOAT_TYPE,                     // float
    STRING_TYPE,                    // string

    //values
    IDENTIFIER,                     // any string of characters
    STRING_LITERAL,                 // "{content}" or '{content}'
    INT_LITERAL,                    // any int
    FLOAT_LITERAL,                  // any float

    //symbols
    OPEN_PAREN, CLOSE_PAREN,        // ()
    OPEN_CURLY, CLOSE_CURLY,        // {}
    OPEN_BRACKET, CLOSE_BRACKET,    // []

    SEMICOLON,                      // ;
    DOT,
    COMMA,                          // ,
    PLUS,                           // +
    MINUS,                          // -
    SLASH,                          // /
    STAR,                           // *
    PERCENT,                        // %
    BAR,                            // |
    UP_ARROW,                       // ^
    GREATER_THAN,                   // >
    LESS_THAN,                      // <
    GREATER_THAN_EQUAL,             // >=
    LESS_THAN_EQUAL,                // <=
    SHIFT_RIGHT,                    // >>
    SHIFT_LEFT,                     // <<
    POW,                            // **

    EQUALS,                         // =
    PLUS_EQUALS,                    // +=
    MINUS_EQUALS,                   // -=
    SLASH_EQUALS,                   // /=
    STAR_EQUALS,                    // *=

    //special characters
    IMAGE_REF,                      // @
    BUILT_IN_VARIABLE_REF,          // $
    SPECIAL_FUNCTION_PREFIX,        // :

    //booleans
    AND,                            // &&
    OR,                             // ||
    BIT_AND,                        // &
    NOT,                            // !
    CMP_EQUALS,                     // ==
    CMP_NOT_EQUALS,                 // !=
    
    //ignored (stripped from lexer output)
    WHITESPACE,
    COMMENT,

    //failsafe
    OTHER
};

struct Token {
    size_t line;
    size_t start_col;
    size_t end_col;
    std::string lexeme;
    TOK_TYPE type;
};

/*
    Lex the program and return the tokens which represent the raw source lines fed into the program.

    Throws a ScribbleErr (err.hpp), with a type of error 
*/
std::vector<Token> lex(std::string &source);
void lex_strip(std::vector<Token> &input); //strip away whitespace and comments