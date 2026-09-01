#include <iostream>
#include <fstream>
#include <string>

#include "lex.hpp"
#include "parse.hpp"
#include "interpreter.hpp"
#include "err.hpp"
#include "debug.hpp"

using namespace std;

bool parse_args(int argc, char** argv, string &res);

int main(int argc, char** argv) {
    string input = "";
    if(!parse_args(argc, argv, input)) {
        cout << "Error reading file" << endl;
        return 0;
    }

    vector<Token> tokens = lex(input);
    try {
        cout << "Lexing..." << endl;
        lex_strip(tokens);
        print_tokens(tokens);
    } catch (ScribbleErr e) {
        PrintSErrMessage(e, input);
    }

    cout << endl;
    cout << endl;

    Nibbler nibbler(&tokens);
    try {
        cout << "Parsing..." << endl;
        AST_Node AST = parse_program(nibbler).second;
        cout << "Done" << endl;
        print_AST(AST);

        if(nibbler.getErrs().size() != 0) {
            for(auto &err : nibbler.getErrs()) {
                PrintSErrMessage(err, input);
            }
            return 0;
        }

        cout << endl;
        cout << endl;

        cout << "Running program" << endl;
        Interpreter::eval(make_shared<AST_Node>(AST), nullptr);
    }
    catch (ScribbleErr e) {
        PrintSErrMessage(e, input);
        for(auto &err : nibbler.getErrs()) {
            PrintSErrMessage(err, input);
        }
    }

    return 0;
}

bool parse_args(int argc, char** argv, string &res) {
    if(argc < 2) return false;

    ifstream f;
    f.open(argv[1]);
    if(!f.is_open()) return false;

    string tmp = "";
    while(getline(f, tmp)) {
        res += tmp + "\n";
    }

    f.close();
    return true;
}