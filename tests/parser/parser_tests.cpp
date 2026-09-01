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
        throw test_fail{"AST type test", "Lexer failed with error: " + e.msg};
    }

    try {
        Nibbler nibbler(&tokens);
        AST_Node res = parser(nibbler).second;

        if(res.type != expected_type) throw test_fail{"AST type test", "Incorrect type produced by parser: " + AST_node_type_to_string(res.type)};
    } catch (ScribbleErr &e) {
        throw test_fail{"AST type test", "Parser failed with error: " + e.msg};
    }
}

void load_parser_tests(vector<test_t> &tests) {
    tests.emplace_back("PARSE: Identifier 1", [&]{ test_return_type("a", parse_identifier, NODE_TYPE::IDENT); });
    tests.emplace_back("PARSE: Identifier 2", [&]{ test_return_type("a.b.c", parse_chained_identifier, NODE_TYPE::IDENT); });
    tests.emplace_back("PARSE: Identifier 3", [&]{ test_return_type("a", parse_chained_identifier, NODE_TYPE::IDENT); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Expression 1", [&]{ test_return_type("(a+b-c)", parse_expression, NODE_TYPE::EXP_ADD); });
    tests.emplace_back("PARSE: Expression 2", [&]{ test_return_type("3+2", parse_expression, NODE_TYPE::EXP_ADD); });
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
    tests.emplace_back("PARSE: Expression 18", [&]{ test_return_type("1", parse_expression, NODE_TYPE::EXP_PRIMARY); });
    tests.emplace_back("PARSE: Expression 19", [&]{ test_return_type("a", parse_expression, NODE_TYPE::VARIABLE_REFERENCE); });
    tests.emplace_back("PARSE: Expression 20", [&]{ test_return_type("true", parse_expression, NODE_TYPE::EXP_PRIMARY); });
    tests.emplace_back("PARSE: Expression 21", [&]{ test_return_type("false", parse_expression, NODE_TYPE::EXP_PRIMARY); });
    tests.emplace_back("PARSE: Expression 22", [&]{ test_return_type("23.34", parse_expression, NODE_TYPE::EXP_PRIMARY); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Loop 1", [&]{ test_return_type("while true {}", parse_loop, NODE_TYPE::LOOP_WHILE); });
    tests.emplace_back("PARSE: Loop 2", [&]{ test_return_type("repeat 2 {}", parse_loop, NODE_TYPE::LOOP_REPEAT); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Branch 1", [&]{ test_return_type("if true {}", parse_branch, NODE_TYPE::BRANCH); });
    tests.emplace_back("PARSE: Branch 2", [&]{ test_return_type("if false {} else {}", parse_branch, NODE_TYPE::BRANCH); });
    tests.emplace_back("PARSE: Branch 3", [&]{ test_return_type("if true {} else if false {}", parse_branch, NODE_TYPE::BRANCH); });
    tests.emplace_back("PARSE: Branch 4", [&]{ test_return_type("if true {} else if false {} else {}", parse_branch, NODE_TYPE::BRANCH); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Function def 1", [&]{ test_return_type("fun test() {}", parse_function_def, NODE_TYPE::FUNCTION_DEF); });
    tests.emplace_back("PARSE: Function def 2", [&]{ test_return_type("fun test(int one, int two) {}", parse_function_def, NODE_TYPE::FUNCTION_DEF); });
    tests.emplace_back("PARSE: Function def 3", [&]{ test_return_type("[quiet] fun test(float one, int two) {}", parse_function_def, NODE_TYPE::FUNCTION_DEF); });
    tests.emplace_back("PARSE: Function def 4", [&]{ test_return_type("[quiet] fun test() {}", parse_function_def, NODE_TYPE::FUNCTION_DEF); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Function modifier 1", [&]{ test_return_type("[quiet]", parse_function_modifier, NODE_TYPE::FUNCTION_MODIFIER); });
    tests.emplace_back("PARSE: Function modifier 2", [&]{ test_return_type("[quiet, test]", parse_function_modifier, NODE_TYPE::FUNCTION_MODIFIER); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Function call 1", [&]{ test_return_type("test()", parse_function_call, NODE_TYPE::FUNCTION_CALL); });
    tests.emplace_back("PARSE: Function call 2", [&]{ test_return_type("another_test()", parse_function_call, NODE_TYPE::FUNCTION_CALL); });
    tests.emplace_back("PARSE: Function call 3", [&]{ test_return_type("test(1, 2, 3)", parse_function_call, NODE_TYPE::FUNCTION_CALL); });
    tests.emplace_back("PARSE: Function call 4", [&]{ test_return_type("test(1.32, \"hello\")", parse_function_call, NODE_TYPE::FUNCTION_CALL); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});
    
    tests.emplace_back("PARSE: Parameters 1", [&]{ test_return_type("float first", parse_parameters, NODE_TYPE::PARAMETERS); });
    tests.emplace_back("PARSE: Parameters 2", [&]{ test_return_type("float first, string second", parse_parameters, NODE_TYPE::PARAMETERS); });
    tests.emplace_back("PARSE: Parameters 3", [&]{ test_return_type("int first, float second", parse_parameters, NODE_TYPE::PARAMETERS); });
    tests.emplace_back("PARSE: Parameters 4", [&]{ test_return_type("int first, float second, string third", parse_parameters, NODE_TYPE::PARAMETERS); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Arguments 1", [&]{ test_return_type("3", parse_arguments, NODE_TYPE::ARGUMENTS); });
    tests.emplace_back("PARSE: Arguments 2", [&]{ test_return_type("a", parse_arguments, NODE_TYPE::ARGUMENTS); });
    tests.emplace_back("PARSE: Arguments 3", [&]{ test_return_type("1, b", parse_arguments, NODE_TYPE::ARGUMENTS); });
    tests.emplace_back("PARSE: Arguments 4", [&]{ test_return_type("1,b", parse_arguments, NODE_TYPE::ARGUMENTS); });
    tests.emplace_back("PARSE: Arguments 5", [&]{ test_return_type("1,2,3", parse_arguments, NODE_TYPE::ARGUMENTS); });
    tests.emplace_back("PARSE: Arguments 6", [&]{ test_return_type("3.23,\"hi\",b", parse_arguments, NODE_TYPE::ARGUMENTS); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Return statement 1", [&]{ test_return_type("return 1", parse_return_statement, NODE_TYPE::RETURN_STATEMENT); });
    tests.emplace_back("PARSE: Return statement 2", [&]{ test_return_type("return 1+3", parse_return_statement, NODE_TYPE::RETURN_STATEMENT); });
    tests.emplace_back("PARSE: Return statement 3", [&]{ test_return_type("return a", parse_return_statement, NODE_TYPE::RETURN_STATEMENT); });
    tests.emplace_back("PARSE: Return statement 4", [&]{ test_return_type("return a+b", parse_return_statement, NODE_TYPE::RETURN_STATEMENT); });
    tests.emplace_back("PARSE: Return statement 5", [&]{ test_return_type("return 2.34", parse_return_statement, NODE_TYPE::RETURN_STATEMENT); });
    tests.emplace_back("PARSE: Return statement 6", [&]{ test_return_type("return test()+3", parse_return_statement, NODE_TYPE::RETURN_STATEMENT); });
    tests.emplace_back("PARSE: Return statement 7", [&]{ test_return_type("return", parse_return_statement, NODE_TYPE::RETURN_STATEMENT); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: vartype 1", [&]{ test_return_type("int", parse_vartype, NODE_TYPE::VAR_TYPE); });
    tests.emplace_back("PARSE: vartype 2", [&]{ test_return_type("float", parse_vartype, NODE_TYPE::VAR_TYPE); });
    tests.emplace_back("PARSE: vartype 3", [&]{ test_return_type("string", parse_vartype, NODE_TYPE::VAR_TYPE); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Variable def 1", [&]{ test_return_type("int i", parse_variable_def, NODE_TYPE::VARIABLE_DEF); });
    tests.emplace_back("PARSE: Variable def 2", [&]{ test_return_type("float i=2", parse_variable_def, NODE_TYPE::VARIABLE_DEF); });
    tests.emplace_back("PARSE: Variable def 3", [&]{ test_return_type("float b=23.43", parse_variable_def, NODE_TYPE::VARIABLE_DEF); });
    tests.emplace_back("PARSE: Variable def 4", [&]{ test_return_type("string s=\"hi\"", parse_variable_def, NODE_TYPE::VARIABLE_DEF); });
    tests.emplace_back("PARSE: Variable def 5", [&]{ test_return_type("string i", parse_variable_def, NODE_TYPE::VARIABLE_DEF); });
    tests.emplace_back("PARSE: Variable def 6", [&]{ test_return_type("int[4,3] b", parse_variable_def, NODE_TYPE::VARIABLE_DEF); });
    tests.emplace_back("PARSE: Variable def 7", [&]{ test_return_type("int a, b, c, d", parse_variable_def, NODE_TYPE::VARIABLE_DEF); });
    tests.emplace_back("PARSE: Variable def 8", [&]{ test_return_type("string a,b,c = \"test\"", parse_variable_def, NODE_TYPE::VARIABLE_DEF); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Variable ref 1", [&]{ test_return_type("a", parse_variable_reference, NODE_TYPE::VARIABLE_REFERENCE); });
    tests.emplace_back("PARSE: Variable ref 2", [&]{ test_return_type("a[1]", parse_variable_reference, NODE_TYPE::VARIABLE_REFERENCE); });
    tests.emplace_back("PARSE: Variable ref 3", [&]{ test_return_type("b[1,2]", parse_variable_reference, NODE_TYPE::VARIABLE_REFERENCE); });
    tests.emplace_back("PARSE: Variable ref 4", [&]{ test_return_type("b[12, 43, 23]", parse_variable_reference, NODE_TYPE::VARIABLE_REFERENCE); });
    tests.emplace_back("PARSE: Variable ref 5", [&]{ test_return_type("a[y, z]", parse_variable_reference, NODE_TYPE::VARIABLE_REFERENCE); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Built-in variable ref 1", [&]{ test_return_type("$test", parse_variable_reference, NODE_TYPE::BUILT_IN_VAR_REFERENCE); });
    tests.emplace_back("PARSE: Built-in variable ref 2", [&]{ test_return_type("$another_test", parse_variable_reference, NODE_TYPE::BUILT_IN_VAR_REFERENCE); });
    tests.emplace_back("PARSE: Built-in variable ref 3", [&]{ test_return_type("$test[1,2]", parse_variable_reference, NODE_TYPE::BUILT_IN_VAR_REFERENCE); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: import statement 1", [&]{ test_return_type("use module as test", parse_import_statement, NODE_TYPE::IMPORT_STATEMENT); });
    tests.emplace_back("PARSE: import statement 2", [&]{ test_return_type("use nested.module as test", parse_import_statement, NODE_TYPE::IMPORT_STATEMENT); });
    tests.emplace_back("PARSE: import statement 3", [&]{ test_return_type("use really.nested.module as test", parse_import_statement, NODE_TYPE::IMPORT_STATEMENT); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Core START", [&]{ test_return_type(":START: {}", parse_core_function, NODE_TYPE::START_FUNC); });
    tests.emplace_back("PARSE: Core UPDATE", [&]{ test_return_type(":UPDATE: {}", parse_core_function, NODE_TYPE::UPDATE_FUNC); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});

    tests.emplace_back("PARSE: Program", [&]{ test_return_type(":START: { int i=0; } :UPDATE: { i++; }", parse_program, NODE_TYPE::PROGRAM); });
}