#include "interpreter.hpp"
#include "debug.hpp"

using namespace Interpreter;

std::vector<SharedSymbol> GlobalValues;

std::shared_ptr<AST_Node> StartFunction = nullptr;
std::shared_ptr<AST_Node> UpdateFunction = nullptr;
std::unordered_map<std::string, std::shared_ptr<AST_Node>> functions;

// Creating child scopes
std::shared_ptr<SymbolTable> newScopeWithParent(std::shared_ptr<SymbolTable> parent);

// Safe way to construct errors
size_t getLine(std::shared_ptr<Token> tok);
size_t getCol(std::shared_ptr<Token> tok); 

AnyValue Interpreter::eval(std::shared_ptr<AST_Node> root, std::shared_ptr<AnyValue> returnContext, std::shared_ptr<SymbolTable> memTable) {
    if(returnContext->type != EVAL_RES_TYPE::None) return *returnContext;

    switch(root->type) {
        case NODE_TYPE::PROGRAM: {
            for(auto& node : root.get()->children)
                eval(node, nullptr, memTable);
            
            if(StartFunction==nullptr) throw ScribbleErr {getLine(root->tok), getCol(root->tok), ":START: function", ERR_TYPE::MISSING_REQUIRED};
            if(UpdateFunction==nullptr) throw ScribbleErr {getLine(root->tok), getCol(root->tok), ":UPDATE: function", ERR_TYPE::MISSING_REQUIRED};

            // Run start function
            for(auto& node : StartFunction->children)
                eval(node, std::make_shared<AnyValue>(), newScopeWithParent(memTable));

            // Run main loop
            // TODO: Include method of gracefully breaking out of this loop through the graphical window
            auto updateFunctionReturnCtx = std::make_shared<AnyValue>();
            while (updateFunctionReturnCtx->type == EVAL_RES_TYPE::None) {
                for(auto& node : UpdateFunction->children)
                    eval(node, updateFunctionReturnCtx, newScopeWithParent(memTable));
            }
            break;
        }

        case NODE_TYPE::VARIABLE_DEF:
            if(returnContext == nullptr) {
                // Store in global memory
            }
            break;

        default:
            log("AST Node not recognized");
    }

    return AnyValue{};
}

// Helper functions -----------------------------------------------------------------------------

std::shared_ptr<SymbolTable> newScopeWithParent(std::shared_ptr<SymbolTable> parent) {
    auto child = std::make_shared<SymbolTable>();
    child->parent = parent;
    return child;
}

size_t getLine(std::shared_ptr<Token> tok) {
    return tok.get() == nullptr ? 0 : tok->line;
}

size_t getCol(std::shared_ptr<Token> tok) {
    return tok.get() == nullptr ? 0 : tok->start_col;
}