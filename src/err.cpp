#include <iostream>
#include "err.hpp"

using namespace std;

string errmsg(ERR_TYPE t);
string get_line_from_source(string &source, size_t line_num);

void PrintSErrMessage(ScribbleErr e, string &source) {
    string col_spacing = "";
    for(int i=0; i<e.col; i++) col_spacing += ' ';

    cout << "----------- Scribble error -----------" << endl;
    cout << endl;
    cout << "  " << e.line << ": " << get_line_from_source(source, e.line) << endl;
    cout << "     " << col_spacing << "^" << endl;
    cout << "  " << errmsg(e.type) << endl;
    if(e.msg.size()!=0) 
        cout << "    " << e.msg << endl;
    cout << "--------------------------------------" << endl;
}

string errmsg(ERR_TYPE t) {
    switch(t) {
        case ERR_TYPE::UNIMPLEMENTED:
            return "NOT IMPLEMENTED";
        case ERR_TYPE::UNKNOWN_CHARACTER:
            return "Uknown character in source";
        case ERR_TYPE::UNCLOSED_QUOTE:
            return "Expected closing quote";
        case ERR_TYPE::UNRECOGNIZED_PATTERN:
            return "Unrecognized pattern in source code";
        case ERR_TYPE::EXPECTED:
            return "Expected:";
        //TODO: add new error messages
        default:
            return "Unimplemented Error Message";
    }
}

string get_line_from_source(string &source, size_t line_num) {
    string::iterator it = source.begin();

    //find start of the line in the source
    size_t curr_line = 0;
    while(it < source.end()) {
        if(curr_line==line_num) break;
        if(*(it++) == '\n') curr_line++;
    }

    if(it >= source.end()) return "";

    string res = "";
    while(it < source.end() && *it != '\n') {
        res += *(it++);
    }

    return res;
}