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

void checkDtype(AnyValue val, EVAL_RES_TYPE dtype, shared_ptr<AST_Node> node);
shared_ptr<void> defaultValueFor(EVAL_RES_TYPE);

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
            InitRuntime();

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

            // get the datatype from the first child
            EVAL_RES_TYPE dtype = dtypeFromIdent(root->children[0]);

            // get every new variable name from the second child onwards
            size_t i=1;
            for(; i<root->children.size() && root->children[i]->type == NODE_TYPE::IDENT; i++) {
                idents.push_back(root->children[1]->tok->lexeme);
            }

            // get the assigned value or the default vale for the new variable
            if(i < root->children.size()) {
                *val = eval(root->children[i], returnContext, memTable);
                checkDtype(*val, dtype, root->children[i]);
            } 
            else {
                *val = AnyValue{{1}, defaultValueFor(dtype), dtype};
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
            //convert node to AnyValue
            switch(root->tok->type) {
                case TOK_TYPE::INT_LITERAL:
                    return AnyValue{{1}, make_shared<SCRIBBLE_NUM_REP>(stoi(root->tok->lexeme)), EVAL_RES_TYPE::Num};
                case TOK_TYPE::FLOAT_LITERAL:
                    return AnyValue{{1}, make_shared<SCRIBBLE_FLOAT_REP>(stof(root->tok->lexeme)), EVAL_RES_TYPE::Float};
                case TOK_TYPE::TRUE:
                    return AnyValue{{1}, make_shared<bool>(true), EVAL_RES_TYPE::Bool};
                case TOK_TYPE::FALSE:
                    return AnyValue{{1}, make_shared<bool>(false), EVAL_RES_TYPE::Bool};
                case TOK_TYPE::STRING_LITERAL:
                    return AnyValue{{1}, make_shared<string>(root->tok->lexeme), EVAL_RES_TYPE::String};
                default:
                    throwScribbleError(root, "Expression primary not recognized", ERR_TYPE::UNEXPECTED_TOKEN);
            }
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


void checkDtype(AnyValue val, EVAL_RES_TYPE dtype, shared_ptr<AST_Node> node) {
    if(val.type != dtype) 
        throwScribbleError(node, "Invalid value for datatype", ERR_TYPE::INVALID_ASSIGNMENT);
}

shared_ptr<void> defaultValueFor(EVAL_RES_TYPE resType) {
    switch(resType) {
        case EVAL_RES_TYPE::None:
            return make_shared<int>(0); // Trying to avoid the existence of nullptrs
        case EVAL_RES_TYPE::Num:
            return make_shared<SCRIBBLE_NUM_REP>(0);
        case EVAL_RES_TYPE::Float:
            return make_shared<SCRIBBLE_FLOAT_REP>(0);
        case EVAL_RES_TYPE::Bool:
            return make_shared<bool>(false);
        case EVAL_RES_TYPE::String:
            return make_shared<string>("");
        case EVAL_RES_TYPE::Object:
            return make_shared<int>(0); // Trying to avoid the existence of nullptrs
        default:
            break; // custom values 
    }

    return make_shared<int>(0);
}

void throwScribbleError(shared_ptr<AST_Node> node, string message, ERR_TYPE type) {
    size_t l = 0, c = 0;

    if(node != nullptr && node->tok != nullptr) {
        l = node->tok->line;
        c = node->tok->start_col;
    }

    throw ScribbleErr {l, c, message, type};
}