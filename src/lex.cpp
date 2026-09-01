#include "lex.hpp"
#include "utils/file_reader.hpp"
#include "err.hpp"

using namespace std;

//helper prototypes
TOK_TYPE handle_alpha(file_reader::file_reader &fr, string &lexeme);
TOK_TYPE handle_digit(file_reader::file_reader &fr, string &lexeme);
TOK_TYPE handle_string(file_reader::file_reader &fr, string &lexeme, char start);
TOK_TYPE handle_comment(file_reader::file_reader &fr);
TOK_TYPE handle_others(file_reader::file_reader &fr, string &lexeme);
TOK_TYPE check_is_keyword(string &lexeme, file_reader::file_reader &fr);

vector<Token> lex(string &source) 
{
    //where we're going to store everything
    vector<Token> tokens;

    file_reader::file_reader fr(&source);
    string lexeme = "";

    while(!fr.empty()) 
    {
        //reset lexeme
        lexeme = "";

        //get what type of token we should have
        TOK_TYPE type = TOK_TYPE::OTHER;
        size_t start_col = fr.loc.col;

        char c = fr.peek();
        if(isalpha(c) || c=='_')
            type = handle_alpha(fr, lexeme);
        else if(isdigit(c))
            type = handle_digit(fr, lexeme);
        else if(c == '"' || c == '\'')
            type = handle_string(fr, lexeme, c);
        else if(c == '#')
            type = handle_comment(fr);
        else
            type = handle_others(fr, lexeme);

        //construct and push token
        Token next;
        next.line = fr.loc.row;
        next.start_col = start_col;
        next.end_col = fr.loc.col;
        next.lexeme = lexeme;
        next.type = type;

        tokens.push_back(next);
    }

    return tokens;
}

//strip away whitespace and comments
void lex_strip(std::vector<Token> &input) {
    for(int i=(int)(input.size()-1); i>=0; i--) {
        if(input[i].type==TOK_TYPE::WHITESPACE || input[i].type==TOK_TYPE::COMMENT) {
            input.erase(input.begin()+i);
        }
    }
}

TOK_TYPE handle_alpha(file_reader::file_reader &fr, string &lexeme) {
    lexeme += fr.next();

    while(!fr.empty() && (isalpha(fr.peek()) || isdigit(fr.peek()) || fr.peek()=='_')) {
        lexeme += fr.next();
    }

    return check_is_keyword(lexeme, fr);
}

TOK_TYPE handle_digit(file_reader::file_reader &fr, string &lexeme) {
    lexeme += fr.next();

    bool found_dot = false;

    while(!fr.empty() && (isdigit(fr.peek()) || fr.peek() == '.')) {
        char next = fr.peek();
        if(next == '.') {
            if(found_dot)
                return TOK_TYPE::FLOAT_LITERAL; //if we already found a dot, return what we currently found
            else
                found_dot = true;
        }

        lexeme += fr.next();
    }

    return found_dot ? TOK_TYPE::FLOAT_LITERAL : TOK_TYPE::INT_LITERAL;
}

TOK_TYPE handle_string(file_reader::file_reader &fr, string &lexeme, char start) {
    lexeme += fr.next();
    
    while(!fr.empty() && fr.peek() != start && fr.peek() != '\n') {
        lexeme += fr.next();
    }

    if(fr.empty() || fr.peek() == '\n') throw (ScribbleErr) { fr.loc.row, fr.loc.col, "", ERR_TYPE::UNCLOSED_QUOTE };

    lexeme += fr.next(); //get the last quote added to the lexeme

    return TOK_TYPE::STRING_LITERAL;
}

TOK_TYPE handle_comment(file_reader::file_reader &fr) {
    fr.next();
    while(!fr.empty() && fr.next() != '\n');
    return TOK_TYPE::COMMENT;
}

TOK_TYPE handle_others(file_reader::file_reader &fr, string &lexeme) {
    char next = fr.next();
    lexeme += next;

    switch(next) {
        case '(': return TOK_TYPE::OPEN_PAREN;
        case ')': return TOK_TYPE::CLOSE_PAREN;
        case '{': return TOK_TYPE::OPEN_CURLY;
        case '}': return TOK_TYPE::CLOSE_CURLY;
        case '[': return TOK_TYPE::OPEN_BRACKET;
        case ']': return TOK_TYPE::CLOSE_BRACKET;
        case ';': return TOK_TYPE::SEMICOLON;
        case '.': return TOK_TYPE::DOT;
        case ',': return TOK_TYPE::COMMA;
        case '%': return TOK_TYPE::PERCENT;
        case '+': 
            if(fr.has_next("=", lexeme, false))
                return TOK_TYPE::PLUS_EQUALS;
            else
                return TOK_TYPE::PLUS;
        case '-':
            if(fr.has_next("=", lexeme, false))    
                return TOK_TYPE::MINUS_EQUALS;
            else
                return TOK_TYPE::MINUS;
        case '/':
            if(fr.has_next("=", lexeme, false))
                return TOK_TYPE::SLASH_EQUALS;
            else
                return TOK_TYPE::SLASH;
        case '*':
            if(fr.has_next("=", lexeme, false))
                return TOK_TYPE::STAR_EQUALS;
            else if(fr.has_next("*", lexeme, false))
                return TOK_TYPE::POW;
            else
                return TOK_TYPE::STAR;
        case '|': 
            if(fr.has_next("|", lexeme, false))    
                return TOK_TYPE::OR;
            else
                return TOK_TYPE::BAR;
        case '>': 
            if(fr.has_next(">", lexeme, false))
                return TOK_TYPE::SHIFT_RIGHT;
            else if(fr.has_next("=", lexeme, false))
                return TOK_TYPE::GREATER_THAN_EQUAL;
            else
                return TOK_TYPE::GREATER_THAN;
        case '<':
            if(fr.has_next("<", lexeme, false))
                return TOK_TYPE::SHIFT_LEFT;
            else if(fr.has_next("=", lexeme, false))
                return TOK_TYPE::LESS_THAN_EQUAL;
            else
                return TOK_TYPE::LESS_THAN;
        case '=':
            if(fr.has_next("=", lexeme, false))
                return TOK_TYPE::CMP_EQUALS;
            else
                return TOK_TYPE::EQUALS;
        case '&':
            if(fr.has_next("&", lexeme, false))
                return TOK_TYPE::AND;
            else
                return TOK_TYPE::BIT_AND;
        case '!': 
            if(fr.has_next("=", lexeme, false))
                return TOK_TYPE::CMP_NOT_EQUALS;
            else
                return TOK_TYPE::NOT;
        case '^': return TOK_TYPE::UP_ARROW;
        case '@': return TOK_TYPE::IMAGE_REF;
        case '$': return TOK_TYPE::BUILT_IN_VARIABLE_REF;
        case ':': return TOK_TYPE::SPECIAL_FUNCTION_PREFIX;
        case ' ':
        case '\n':
        case '\t':
            return TOK_TYPE::WHITESPACE;
        default:
            throw (ScribbleErr) { fr.loc.row, fr.loc.col, "", ERR_TYPE::UNKNOWN_CHARACTER };
            return TOK_TYPE::OTHER;
    }
}

/*
    Check if a string of characters is a keyword in the language or just a regular identifier
*/
TOK_TYPE check_is_keyword(string &lexeme, file_reader::file_reader &fr) {
    if(lexeme == "use")
        return TOK_TYPE::USE;
    else if(lexeme == "as")
        return TOK_TYPE::AS;
    else if(lexeme == "if")
        return TOK_TYPE::IF;
    else if(lexeme == "else") {
        //check for else in "else if"
        if(fr.has_next(" if", lexeme)) { 
            return TOK_TYPE::ELSE_IF;
        }
        return TOK_TYPE::ELSE;
    }
    else if(lexeme == "while")
        return TOK_TYPE::WHILE;
    else if(lexeme == "repeat")
        return TOK_TYPE::REPEAT;
    else if(lexeme == "true")
        return TOK_TYPE::TRUE;
    else if(lexeme == "false")
        return TOK_TYPE::FALSE;
    else if(lexeme == "fun")
        return TOK_TYPE::FUNCTION_DEFINE;
    else if(lexeme == "num")
        return TOK_TYPE::NUMBER_TYPE;
    else if(lexeme == "float")
        return TOK_TYPE::FLOAT_TYPE;
    else if(lexeme == "bool")
        return TOK_TYPE::BOOL_TYPE;
    else if(lexeme == "string")
        return TOK_TYPE::STRING_TYPE;
    else if(lexeme == "return")
        return TOK_TYPE::RETURN;
    
    return TOK_TYPE::IDENTIFIER;
}