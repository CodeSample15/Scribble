#include "../../include/lex.hpp"
#include "../../include/debug.hpp"
#include "../../include/err.hpp"
#include "parser_tests.hpp"

using namespace std;

void test_return_type(string input, std::function<AST_Nib_Pair_t(Nibbler)> parser, NODE_TYPE expected_type) {
    vector<Token> tokens;

    try {
        tokens = lex(input);
        lex_strip(tokens);
    } catch (ScribbleErr &e) {
        throw test_fail{"AST type test", "Lex failed with error: " + e.msg};
    }

    try {
        Nibbler nibbler(&tokens);
        AST_Node res = parser(nibbler).second;

        if(res.type != expected_type) throw test_fail{"AST type test", "Incorrect type produced by parser: " + AST_node_type_to_string(res.type)};
    } catch (ScribbleErr &e) {
        throw test_fail{"AST type test", "Lex failed with error: " + e.msg};
    }
}

void load_parser_tests(vector<test_t> &tests) {
    tests.emplace_back("PARSE: Identifier 1", [&]{ test_return_type("a", parse_identifier, NODE_TYPE::IDENT); });
    tests.emplace_back("PARSE: Identifier 2", [&]{ test_return_type("a.b.c", parse_chained_identifier, NODE_TYPE::IDENT); });
    tests.emplace_back("PARSE: Identifier 3", [&]{ test_return_type("a", parse_chained_identifier, NODE_TYPE::IDENT); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Expression 1", [&]{ test_return_type("(a+b-c)", parse_expression, NODE_TYPE::EXP_ADD); });
    tests.emplace_back("PARSE: Expression 2", [&]{ test_return_type("2", parse_expression, NODE_TYPE::EXP_PRIMARY); });
    tests.emplace_back("PARSE: Expression 3", [&]{ test_return_type("3*4", parse_expression, NODE_TYPE::EXP_MULT); });
    tests.emplace_back("PARSE: Expression 4", [&]{ test_return_type("2/4", parse_expression, NODE_TYPE::EXP_MULT); });
    tests.emplace_back("PARSE: Expression 5", [&]{ test_return_type("2+2", parse_expression, NODE_TYPE::EXP_ADD); });
    tests.emplace_back("PARSE: Expression 6", [&]{ test_return_type("2-2", parse_expression, NODE_TYPE::EXP_ADD); });
    tests.emplace_back("PARSE: Expression 7", [&]{ test_return_type("2^2", parse_expression, NODE_TYPE::EXP_XOR); });
    tests.emplace_back("PARSE: Expression 8", [&]{ test_return_type("2&&2", parse_expression, NODE_TYPE::EXP_ANDL); });
    tests.emplace_back("PARSE: Expression 9", [&]{ test_return_type("2||2", parse_expression, NODE_TYPE::EXP_ORL); });
    tests.emplace_back("PARSE: Expression 10", [&]{ test_return_type("a|b", parse_expression, NODE_TYPE::EXP_OR); });
    tests.emplace_back("PARSE: Expression 11", [&]{ test_return_type("2&2", parse_expression, NODE_TYPE::EXP_AND); });
    tests.emplace_back("PARSE: Expression 12", [&]{ test_return_type("2+(43/4)", parse_expression, NODE_TYPE::EXP_ADD); });
    tests.emplace_back("PARSE: Expression 13", [&]{ test_return_type("a<<4", parse_expression, NODE_TYPE::EXP_SHFT); });
    tests.emplace_back("PARSE: Expression 14", [&]{ test_return_type("a>>3", parse_expression, NODE_TYPE::EXP_SHFT); });
    tests.emplace_back("PARSE: Expression 15", [&]{ test_return_type("a>b", parse_expression, NODE_TYPE::EXP_CMP); });
    tests.emplace_back("PARSE: Expression 16", [&]{ test_return_type("a<b", parse_expression, NODE_TYPE::EXP_CMP); });
    tests.emplace_back("PARSE: Expression 17", [&]{ test_return_type("a<=b", parse_expression, NODE_TYPE::EXP_CMP); });
    tests.emplace_back("PARSE: Expression 18", [&]{ test_return_type("a>=b", parse_expression, NODE_TYPE::EXP_CMP); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});
}