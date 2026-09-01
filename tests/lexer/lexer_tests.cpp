#include "../../include/lex.hpp"
#include "../../include/debug.hpp"
#include "../../include/err.hpp"
#include "lexer_tests.hpp"

using namespace std;

void test_type_lex(string input, TOK_TYPE expected_type, string expected_lexeme, bool should_fail=false) {
    vector<Token> out;

    try {
        out = lex(input);

        if(should_fail) throw (test_fail) {"Type/Lexeme test", "Lex didn't fail when it should have.\n"};
    } catch(ScribbleErr &e) {
        if(should_fail) return;
        PrintSErrMessage(e, input);
        throw (test_fail) {"Type/Lexeme test", "Lex failed"};
    }

    if(out.size() == 0) 
        throw (test_fail) {"Type/Lexeme test", "Empty output"};
    if(out[0].type != expected_type)
        throw (test_fail) {"Type/Lexeme test", "Expected type |" + tok_type_to_string(expected_type) + "|, got |" + tok_type_to_string(out[0].type) + "|"};
    if(out[0].lexeme != expected_lexeme)
        throw (test_fail) {"Type/Lexeme test", "Expected lexeme '" + expected_lexeme + "', got '" + out[0].lexeme + "'"};
}

void test_multiple_types(string input, vector<TOK_TYPE> expected) {
    vector<Token> out;

    try {
        out = lex(input);
    } catch(ScribbleErr &e) {
        PrintSErrMessage(e, input);
        throw (test_fail) {"Type/Lexeme test", "Lex failed"};
    }

    if(out.size() != expected.size())
        throw (test_fail) {"Type test", "Out size (" + to_string(out.size()) + ") does not match expected size (" + to_string(expected.size()) + ")"};

    for(size_t i=0; i<out.size(); i++) {
        if(out[i].type != expected[i])
            throw (test_fail) {"Type test", "Incorrect type produced from lexer. Expected |" + tok_type_to_string(expected[0]) + "|, got |" + tok_type_to_string(out[i].type) + "|"};
    }
}

void load_lexer_tests(vector<test_t> &tests) {
    //testing literals
    tests.emplace_back("LEX: Identifier 1", [&]{ test_type_lex("x", TOK_TYPE::IDENTIFIER, "x"); });
    tests.emplace_back("LEX: Identifier 2", [&]{ test_type_lex("h3ll0", TOK_TYPE::IDENTIFIER, "h3ll0"); });
    tests.emplace_back("LEX: Identifier 3", [&]{ test_type_lex("_h3ll0", TOK_TYPE::IDENTIFIER, "_h3ll0"); });
    tests.emplace_back("LEX: Identifier 4", [&]{ test_type_lex("x_y", TOK_TYPE::IDENTIFIER, "x_y"); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("LEX: Digit 1", [&]{ test_type_lex("23", TOK_TYPE::INT_LITERAL, "23"); });
    tests.emplace_back("LEX: Digit 2", [&]{ test_type_lex("23.3", TOK_TYPE::FLOAT_LITERAL, "23.3"); });
    tests.emplace_back("LEX: Digit 3", [&]{ test_type_lex("23.", TOK_TYPE::FLOAT_LITERAL, "23."); });
    tests.emplace_back("LEX: Digit 4", [&]{ test_type_lex("23.3", TOK_TYPE::FLOAT_LITERAL, "23.3"); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("LEX: String literal", [&]{ test_type_lex("\"Hello World!\"", TOK_TYPE::STRING_LITERAL, "\"Hello World!\""); });
    tests.emplace_back("LEX: String literal - unclosed", [&]{ test_type_lex("\"Hello World!", TOK_TYPE::STRING_LITERAL, "", true); });
    tests.emplace_back("LEX: String literal - newline unclosed", [&]{ test_type_lex("\"Hello Wor\nld!\"", TOK_TYPE::STRING_LITERAL, "", true); });
    tests.emplace_back("LEX: String literal - ' string", [&]{ test_type_lex("'Hello World'", TOK_TYPE::STRING_LITERAL, "'Hello World'"); });
    tests.emplace_back("LEX: String literal - \" ignored in ' string", [&]{ test_type_lex("'Hello \"World\"'", TOK_TYPE::STRING_LITERAL, "'Hello \"World\"'"); });
    tests.emplace_back("LEX: String literal - ' ignored in \" string", [&]{ test_type_lex("\"Hello 'World'\"", TOK_TYPE::STRING_LITERAL, "\"Hello 'World'\""); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    //testing keywords
    tests.emplace_back("LEX: keyword: use", [&]{ test_type_lex("use", TOK_TYPE::USE, "use"); });
    tests.emplace_back("LEX: keyword: as", [&]{ test_type_lex("as", TOK_TYPE::AS, "as"); });
    tests.emplace_back("LEX: keyword: if", [&]{ test_type_lex("if", TOK_TYPE::IF, "if"); });
    tests.emplace_back("LEX: keyword: else if", [&]{ test_type_lex("else if", TOK_TYPE::ELSE_IF, "else if"); });
    tests.emplace_back("LEX: keyword: else", [&]{ test_type_lex("else", TOK_TYPE::ELSE, "else"); });
    tests.emplace_back("LEX: keyword: while", [&]{ test_type_lex("while", TOK_TYPE::WHILE, "while"); });
    tests.emplace_back("LEX: keyword: repeat", [&]{ test_type_lex("repeat", TOK_TYPE::REPEAT, "repeat"); });
    tests.emplace_back("LEX: keyword: true", [&]{ test_type_lex("true", TOK_TYPE::TRUE, "true"); });
    tests.emplace_back("LEX: keyword: false", [&]{ test_type_lex("false", TOK_TYPE::FALSE, "false"); });
    tests.emplace_back("LEX: keyword: fun", [&]{ test_type_lex("fun", TOK_TYPE::FUNCTION_DEFINE, "fun"); });
    tests.emplace_back("LEX: keyword: num", [&]{ test_type_lex("num", TOK_TYPE::NUMBER_TYPE, "num"); });
    tests.emplace_back("LEX: keyword: float", [&]{ test_type_lex("float", TOK_TYPE::FLOAT_TYPE, "float"); });
    tests.emplace_back("LEX: keyword: bool", [&]{ test_type_lex("bool", TOK_TYPE::BOOL_TYPE, "bool"); });
    tests.emplace_back("LEX: keyword: string", [&]{ test_type_lex("string", TOK_TYPE::STRING_TYPE, "string"); });
    tests.emplace_back("LEX: keyword: return", [&]{ test_type_lex("return", TOK_TYPE::RETURN, "return"); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    //make sure things like 'else if' only lex as an else-if token if there are no invalid characters following the pattern
    tests.emplace_back("LEX: has_next can't be tricked", [&]{ 
        test_multiple_types("else iff", {
            TOK_TYPE::ELSE, TOK_TYPE::WHITESPACE, TOK_TYPE::IDENTIFIER
        });
    });

    tests.emplace_back("LEX: has_next ignores non alpha", [&]{ 
        test_multiple_types("else if{", {
            TOK_TYPE::ELSE_IF, TOK_TYPE::OPEN_CURLY
        }); 
    });

    tests.emplace_back("LEX: has_next ignores non alpha 2", [&]{ 
        test_multiple_types("else if {", {
            TOK_TYPE::ELSE_IF, TOK_TYPE::WHITESPACE, TOK_TYPE::OPEN_CURLY
        }); 
    });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    //testing sequences
    tests.emplace_back("LEX: symbols", [&]{ 
        test_multiple_types(".(){}[];,+-/*%|^><>=<=>><<=+=-=/=*=@$:&&||! == !=**&", {
            TOK_TYPE::DOT,
            TOK_TYPE::OPEN_PAREN, TOK_TYPE::CLOSE_PAREN,
            TOK_TYPE::OPEN_CURLY, TOK_TYPE::CLOSE_CURLY,
            TOK_TYPE::OPEN_BRACKET, TOK_TYPE::CLOSE_BRACKET,
            TOK_TYPE::SEMICOLON,
            TOK_TYPE::COMMA,
            TOK_TYPE::PLUS, TOK_TYPE::MINUS, TOK_TYPE::SLASH, TOK_TYPE::STAR, TOK_TYPE::PERCENT,
            TOK_TYPE::BAR,
            TOK_TYPE::UP_ARROW,
            TOK_TYPE::GREATER_THAN, TOK_TYPE::LESS_THAN,
            TOK_TYPE::GREATER_THAN_EQUAL, TOK_TYPE::LESS_THAN_EQUAL,
            TOK_TYPE::SHIFT_RIGHT, TOK_TYPE::SHIFT_LEFT,
            TOK_TYPE::EQUALS,
            TOK_TYPE::PLUS_EQUALS, TOK_TYPE::MINUS_EQUALS, TOK_TYPE::SLASH_EQUALS, TOK_TYPE::STAR_EQUALS,
            TOK_TYPE::IMAGE_REF,
            TOK_TYPE::BUILT_IN_VARIABLE_REF,
            TOK_TYPE::SPECIAL_FUNCTION_PREFIX,
            TOK_TYPE::AND, TOK_TYPE::OR, TOK_TYPE::NOT, 
            TOK_TYPE::WHITESPACE,
            TOK_TYPE::CMP_EQUALS, 
            TOK_TYPE::WHITESPACE,
            TOK_TYPE::CMP_NOT_EQUALS,
            TOK_TYPE::POW,
            TOK_TYPE::BIT_AND
        });
    });

    tests.emplace_back("LEX: alpha-symbol combination", [&]{
        test_multiple_types("a>=b", {
            TOK_TYPE::IDENTIFIER, TOK_TYPE::GREATER_THAN_EQUAL, TOK_TYPE::IDENTIFIER
        });
    });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    //testing comments
    tests.emplace_back("LEX: comments work in single line", [&]{ 
        test_multiple_types("#hi there", {
            TOK_TYPE::COMMENT
        }); 
    });

    tests.emplace_back("LEX: comments stop after line", [&]{ 
        test_multiple_types("#hi there\nx", {
            TOK_TYPE::COMMENT, TOK_TYPE::IDENTIFIER
        }); 
    });

    tests.emplace_back("LEX: comments work with tokens on same line", [&]{ 
        test_multiple_types("+#hi there\nx", {
            TOK_TYPE::PLUS, TOK_TYPE::COMMENT, TOK_TYPE::IDENTIFIER
        }); 
    });

    tests.emplace_back("LEX: comments work on new line", [&]{ 
        test_multiple_types("x\n#hi there\nx", {
            TOK_TYPE::IDENTIFIER, TOK_TYPE::WHITESPACE, TOK_TYPE::COMMENT, TOK_TYPE::IDENTIFIER
        }); 
    });

    tests.emplace_back("LEX: multiple comments", [&]{ 
        test_multiple_types("#comment 1\n#hi there\nx", {
            TOK_TYPE::COMMENT, TOK_TYPE::COMMENT, TOK_TYPE::IDENTIFIER
        }); 
    });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});
}