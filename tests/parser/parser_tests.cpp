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
    tests.emplace_back("Identifier 1", [&]{ test_return_type("a", parse_identifier, NODE_TYPE::IDENT); });
}