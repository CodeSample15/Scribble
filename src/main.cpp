#include <iostream>
#include <fstream>
#include <string>

#include "lex.hpp"
#include "parse.hpp"
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

    try {
        cout << "Lexing..." << endl;
        vector<Token> tokens = lex(input);
        lex_strip(tokens);
        print_tokens(tokens);

        cout << endl;
        cout << endl;

        cout << "Parsing..." << endl;
        Nibbler nibbler(&tokens);
        AST_Node AST = parse_program(nibbler).second;
        cout << "Done" << endl;
        print_AST(AST);
    }
    catch (ScribbleErr e) {
        PrintSErrMessage(e, input);
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