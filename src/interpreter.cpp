#include "interpreter.hpp"
#include "debug.hpp"

using namespace Interpreter;

std::vector<SharedSymbol> GlobalValues;

std::shared_ptr<AST_Node> Interpreter::StartFunction = nullptr;
std::shared_ptr<AST_Node> Interpreter::UpdateFunction = nullptr;
std::unordered_map<std::string, std::shared_ptr<AST_Node>> functions;

// Creating child scopes
std::shared_ptr<SymbolTable> newScopeWithParent(std::shared_ptr<SymbolTable> parent);

// Safe way to construct errors
size_t getLine(std::shared_ptr<Token> tok);
size_t getCol(std::shared_ptr<Token> tok); 

AnyValue Interpreter::eval(std::shared_ptr<AST_Node> root, std::shared_ptr<AnyValue> returnContext, std::shared_ptr<SymbolTable> memTable) {
    if(returnContext != nullptr && returnContext->type != EVAL_RES_TYPE::None) return *returnContext;

    switch(root->type) {
        case NODE_TYPE::PROGRAM: {
            for(auto& node : root.get()->children)
                eval(node, nullptr, memTable);
            
            if(StartFunction==nullptr) throw ScribbleErr {getLine(root->tok), getCol(root->tok), ":START: function", ERR_TYPE::MISSING_REQUIRED};
            if(UpdateFunction==nullptr) throw ScribbleErr {getLine(root->tok), getCol(root->tok), ":UPDATE: function", ERR_TYPE::MISSING_REQUIRED};

            // Run start function
            eval(StartFunction->children[0], std::make_shared<AnyValue>(), newScopeWithParent(memTable));

            // Run main loop
            // TODO: Include method of gracefully breaking out of this loop through the graphical window
            auto updateFunctionReturnCtx = std::make_shared<AnyValue>();
            while (updateFunctionReturnCtx->type == EVAL_RES_TYPE::None) {
                eval(UpdateFunction->children[0], updateFunctionReturnCtx, newScopeWithParent(memTable));
            }
            break;
        }

        case NODE_TYPE::IMPORT_STATEMENT:
            break;

        case NODE_TYPE::VARIABLE_DEF: {


            if(returnContext == nullptr) {
                // Store in global memory (we're not in a function)

            }
            else {
                // Store in memory table
            }
            break;
        }

        case NODE_TYPE::VARIABLE_ASSIGN:
            break;

        case NODE_TYPE::VARIABLE_REFERENCE:
            break;

        case NODE_TYPE::START_FUNC:
            StartFunction = root;
            break;

        case NODE_TYPE::UPDATE_FUNC:
            UpdateFunction = root;
            break;

        case NODE_TYPE::BODY:
            break;

        case NODE_TYPE::BRANCH:
            break;

        case NODE_TYPE::LOOP_REPEAT:
            break;

        case NODE_TYPE::LOOP_WHILE:
            break;

        case NODE_TYPE::FUNCTION_DEF:
            break;

        case NODE_TYPE::FUNCTION_CALL:
            break;

        case NODE_TYPE::RETURN_STATEMENT:
            break;

        case NODE_TYPE::EXP_ORL:
            break;

        case NODE_TYPE::EXP_ANDL:
            break;

        case NODE_TYPE::EXP_OR:
            break;

        case NODE_TYPE::EXP_XOR:
            break;

        case NODE_TYPE::EXP_AND:
            break;

        case NODE_TYPE::EXP_EQ:
            break;

        case NODE_TYPE::EXP_CMP:
            break;

        case NODE_TYPE::EXP_SHFT:
            break;

        case NODE_TYPE::EXP_ADD:
            break;

        case NODE_TYPE::EXP_MULT:
            break;

        case NODE_TYPE::EXP_POW:
            break;

        case NODE_TYPE::EXP_NOT:
            break;

        case NODE_TYPE::EXP_PRIMARY:
            break;

        default:
            log("Unexpected AST node reached: " + AST_node_type_to_string(root->type));
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