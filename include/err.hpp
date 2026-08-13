#pragma once

#include <string>

typedef enum {
    // general errors
    UNIMPLEMENTED,

    // lexer errors
    UNKNOWN_CHARACTER,
    UNCLOSED_QUOTE,
    UNRECOGNIZED_PATTERN,

    // parser errors
    EXPECTED,
    EOT, // end of tokens

    // runtime errors
    MISSING_REQUIRED,   // missing a required function
    INVALID_FUN_CALL,   // invalid function call
    UNKNOWN_DTYPE,      // unknown dtype referenced
    INVALID_ASSIGNMENT, // invalid value assignment
    INVALID_SYMBOL,     // symbol not found
} ERR_TYPE;

/*
    This struct can get thrown by the lexer, parser, or runner to signal an unrecoverable error
    was encountered by the program.
    Line, Col, Type 
*/
struct ScribbleErr {
    size_t line;
    size_t col;
    std::string msg;
    ERR_TYPE type;
};

void PrintSErrMessage(ScribbleErr err, std::string &source);