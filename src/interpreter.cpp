#include "interpreter.hpp"
#include "debug.hpp"

using namespace Interpreter;
using namespace std;

vector<SharedSymbol> GlobalValues;

shared_ptr<AST_Node> Interpreter::StartFunction = nullptr;
shared_ptr<AST_Node> Interpreter::UpdateFunction = nullptr;
unordered_map<string, shared_ptr<AST_Node>> Interpreter::Functions;
unordered_map<string, EVAL_RES_TYPE> Interpreter::ValidDTypes;

// Helper functions
shared_ptr<SymbolTable> newScopeWithParent(shared_ptr<SymbolTable> parent);
EVAL_RES_TYPE dtypeFromIdent(shared_ptr<AST_Node> node);
AnyValue defaultValueFor(EVAL_RES_TYPE);

void throwScribbleError(shared_ptr<AST_Node> node, string message, ERR_TYPE type);

void Interpreter::InitRuntime() {
    StartFunction = nullptr;
    UpdateFunction = nullptr;

    Functions.clear();
    ValidDTypes.clear();

    ValidDTypes = {
        {"num", EVAL_RES_TYPE::Num},
        {"float", EVAL_RES_TYPE::Float},
        {"string", EVAL_RES_TYPE::String},
        {"Object", EVAL_RES_TYPE::Object},
    };
}

AnyValue Interpreter::eval(shared_ptr<AST_Node> root, shared_ptr<AnyValue> returnContext, shared_ptr<SymbolTable> memTable) {
    if(returnContext != nullptr && returnContext->type != EVAL_RES_TYPE::None) 
        return *returnContext;

    switch(root->type) {
        case NODE_TYPE::PROGRAM: {
            for(auto& node : root.get()->children)
                eval(node, nullptr, memTable);
            
            if(StartFunction==nullptr) throwScribbleError(root, ":START: function", ERR_TYPE::MISSING_REQUIRED);
            if(UpdateFunction==nullptr) throwScribbleError(root, ":UPDATE: function", ERR_TYPE::MISSING_REQUIRED);

            // Run start function
            eval(StartFunction->children[0], make_shared<AnyValue>(), newScopeWithParent(memTable));

            // Run main loop
            // TODO: Include method of gracefully breaking out of this loop through the graphical window
            auto updateFunctionReturnCtx = make_shared<AnyValue>();
            while (updateFunctionReturnCtx->type == EVAL_RES_TYPE::None) {
                eval(UpdateFunction->children[0], updateFunctionReturnCtx, newScopeWithParent(memTable));
            }
            break;
        }

        case NODE_TYPE::IMPORT_STATEMENT:
            // TODO
            break;

        case NODE_TYPE::VARIABLE_DEF: {
            vector<string> idents;
            auto val = make_shared<AnyValue>();

            size_t i=1;
            for(; i<root->children.size() && root->children[i]->type == NODE_TYPE::IDENT; i++) {
                idents.push_back(root->children[1]->tok->lexeme);
            }

            EVAL_RES_TYPE dtype = dtypeFromIdent(root->children[0]);
            if(i < root->children.size()) {
                *val = eval(root->children[i], returnContext, memTable);

                if(val->type != dtype) 
                    throwScribbleError(root->children[i], "Invalid value for datatype", ERR_TYPE::INVALID_ASSIGNMENT);
            } else {
                *val = defaultValueFor(dtype);
            }

            if(returnContext == nullptr) {
                // Store in global memory (we're not in a function)
                // TODO: global memory space will have to be allocated once the total number of global variables is known
            }
            else {
                // Store in the memory table (we're in a function)
                for(auto& i : idents) {
                    memTable->values.emplace_back(pair<string, shared_ptr<AnyValue>> {i, val});
                }
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
            for(auto& node : root->children)
                eval(node, returnContext, memTable);
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

shared_ptr<SymbolTable> newScopeWithParent(shared_ptr<SymbolTable> parent) {
    auto child = make_shared<SymbolTable>();
    child->parent = parent;
    return child;
}

EVAL_RES_TYPE dtypeFromIdent(shared_ptr<AST_Node> node) {
    if(node == nullptr || node->type != NODE_TYPE::VAR_TYPE)
        throwScribbleError(node, "Variable type", ERR_TYPE::EXPECTED);

    auto it = ValidDTypes.find(node->tok->lexeme);

    if(it == ValidDTypes.end()) 
        throwScribbleError(node, "Data type not recognized", ERR_TYPE::UNKNOWN_DTYPE);

    return it->second;
}

AnyValue defaultValueFor(EVAL_RES_TYPE resType) {
    return AnyValue{};
}

void throwScribbleError(shared_ptr<AST_Node> node, string message, ERR_TYPE type) {
    size_t l = 0, c = 0;

    if(node != nullptr && node->tok != nullptr) {
        l = node->tok->line;
        c = node->tok->start_col;
    }

    throw ScribbleErr {l, c, message, type};
}