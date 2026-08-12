#include "interpreter.hpp"
#include "debug.hpp"

using namespace Interpreter;

std::vector<SharedSymbol> GlobalValues;

std::shared_ptr<AST_Node> StartFunction = nullptr;
std::shared_ptr<AST_Node> UpdateFunction = nullptr;
std::unordered_map<std::string, std::shared_ptr<AST_Node>> functions;

// Safe way to construct errors
size_t getLine(std::shared_ptr<Token> tok);
size_t getCol(std::shared_ptr<Token> tok); 

AnyValue Interpreter::eval(std::shared_ptr<AST_Node> root, std::shared_ptr<SymbolTable> memTable) {
    switch(root->type) {
        case NODE_TYPE::PROGRAM:
            for(auto& node : root.get()->children)
                eval(node, memTable);
            
            if(StartFunction==nullptr) throw ScribbleErr {getLine(root->tok), getCol(root->tok), ":START: function", ERR_TYPE::MISSING_REQUIRED};
            if(UpdateFunction==nullptr) throw ScribbleErr {getLine(root->tok), getCol(root->tok), ":UPDATE: function", ERR_TYPE::MISSING_REQUIRED};

            // Run 
            for(auto& node : StartFunction->children) {
                eval(node, memTable);
            }

            //
            break;
        default:
            log("AST Node not recognized");
    }

    return AnyValue{};
}

// Helper functions to make my code at least a little clean
size_t getLine(std::shared_ptr<Token> tok) {
    return tok.get() == nullptr ? 0 : tok->line;
}

size_t getCol(std::shared_ptr<Token> tok) {
    return tok.get() == nullptr ? 0 : tok->start_col;
}