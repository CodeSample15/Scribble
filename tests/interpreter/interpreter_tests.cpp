#include <string>
#include <fstream>
#include <iostream>

#include "../../include/lex.hpp"
#include "../../include/parse.hpp"
#include "../../include/interpreter.hpp"
#include "../../include/utils/AST.hpp"
#include "interpreter_tests.hpp"

using namespace std;

shared_ptr<AST_Node> load_from_file(string filename) {
    string input = "";

    try {
        ifstream f;
        f.open(filename);
        if(!f.is_open()) {
            cout << "Unable to open source file '" << filename << "'" << endl;
            return make_shared<AST_Node>();
        }

        string tmp = "";
        while(getline(f, tmp))
            input += tmp + "\n";
        f.close();

        vector<Token> tokens = lex(input);
        lex_strip(tokens);

        Nibbler nibbler(&tokens);
        AST_Node AST = parse_program(nibbler).second;

        if(nibbler.getErrs().size() != 0) {
            throw nibbler.getErrs()[0];
        }

        return make_shared<AST_Node>(AST);
    } catch(ScribbleErr &e) {
        PrintSErrMessage(e, input);
    }

    return make_shared<AST_Node>(AST_Node{NODE_TYPE::NON});
}

void test_function(string funName, shared_ptr<AST_Node> program) {
    if(program->type == NODE_TYPE::NON)
        throw test_fail{"Interpreter test '" + funName + "'", "Program failed to compile"};

    for(auto &function : program->children) {
        if(function->type == NODE_TYPE::FUNCTION_DEF && function->children[0]->tok->lexeme == funName) {
            try {
                auto rtnCtx = make_shared<Interpreter::AnyValue>();
                Interpreter::eval(
                    function->children[1], // evaluate the body of the function
                    rtnCtx,
                    make_shared<Interpreter::SymbolTable>());
                
                if(rtnCtx == nullptr)
                    throw test_fail{"Interpreter test '" + funName + "'", "Didn't return anything!"};
                if(rtnCtx->type != Interpreter::EVAL_RES_TYPE::Num || *(SCRIBBLE_NUM_REP*)rtnCtx->value.get() != 0)
                    throw test_fail{"Interpreter test '" + funName + "'", "Bad return"};
            } catch (ScribbleErr &e) {
                throw test_fail{"Interpreter test '" + funName + "'", "Interpreter failed with error: " + e.msg};
            }

            return;
        }
    }
}

void load_interpreter_tests(vector<test_t> &tests) {
    shared_ptr<AST_Node> program = load_from_file("../interpreter/tests.sb"); // TODO: file should be loaded smarter so that the executable can be ran anywhere
    Interpreter::InitRuntime();

    tests.emplace_back("INTERPRETER: equations", [=]{ test_function("test_equations", program); });
    tests.emplace_back("INTERPRETER: built in functions", [=]{ test_function("test_built_in_functions", program); });
    tests.emplace_back("INTERPRETER: branching", [=]{ test_function("test_branching", program); });
    tests.emplace_back("INTERPRETER: comments", [=]{ test_function("test_comments", program); });
    tests.emplace_back("INTERPRETER: loops", [=]{ test_function("test_loops", program); });
    tests.emplace_back("INTERPRETER: variables", [=]{ test_function("test_variables", program); });
    tests.emplace_back("INTERPRETER: strings - concatenation", [=]{ test_function("test_string_concat", program); });
    tests.emplace_back("INTERPRETER: strings - assignment", [=]{ test_function("test_string_assign", program); });
    tests.emplace_back("INTERPRETER: strings - compare", [=]{ test_function("test_string_compare", program); });
    tests.emplace_back("INTERPRETER: bitwise ops", [=]{ test_function("test_bitwise", program); });
    tests.emplace_back("INTERPRETER: arrays", [=]{ test_function("test_arrays", program); });

    tests.emplace_back(TEST_NAME_FOR_SPACE, []{});
}