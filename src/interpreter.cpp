#include <cmath>

#include "interpreter.hpp"
#include "runtime/built_in.hpp"
#include "debug.hpp"

using namespace Interpreter;
using namespace std;

vector<SharedSymbol> Interpreter::GlobalValues;
SymbolTableValues Interpreter::BuiltInVariables; 

shared_ptr<AST_Node> Interpreter::StartFunction = nullptr;
shared_ptr<AST_Node> Interpreter::UpdateFunction = nullptr;
unordered_map<string, shared_ptr<AST_Node>> Interpreter::Functions;
unordered_map<string, EVAL_RES_TYPE> Interpreter::ValidDTypes;

// Helper functions
shared_ptr<SymbolTable> newScopeWithParent(shared_ptr<SymbolTable> parent);
EVAL_RES_TYPE dtypeFromIdent(shared_ptr<AST_Node> node);

void checkForAllowedDtype(AnyValue val, vector<EVAL_RES_TYPE> dtypes, shared_ptr<AST_Node> &node);
bool isPrimitive(EVAL_RES_TYPE t); // returns true if the passed type is a primitive type
void checkSingleVal(AnyValue val, shared_ptr<AST_Node> &node);
shared_ptr<void> defaultValueFor(EVAL_RES_TYPE);
pair<shared_ptr<void>, EVAL_RES_TYPE> castNumValue(double val, EVAL_RES_TYPE type1, EVAL_RES_TYPE type2);
double extractNumValue(AnyValue &val, shared_ptr<AST_Node> &node);
void castAndAssign(AnyValue &val, double newVal);

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

    BuiltIn::init(BuiltInVariables);
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
            updateFunctionReturnCtx->type = EVAL_RES_TYPE::None;
            while (updateFunctionReturnCtx->type == EVAL_RES_TYPE::None) {
                eval(UpdateFunction->children[0], updateFunctionReturnCtx, newScopeWithParent(memTable));
                BuiltIn::update(BuiltInVariables);
            }
            break;
        }

        case NODE_TYPE::IMPORT_STATEMENT:
            // TODO
            break;

        case NODE_TYPE::VARIABLE_DEF: {
            vector<string> idents;
            AnyValue val;

            // get the datatype from the first child
            EVAL_RES_TYPE dtype = dtypeFromIdent(root->children[0]);

            // get every new variable name from the second child onwards
            size_t i=1;
            for(; i<root->children.size() && root->children[i]->type == NODE_TYPE::IDENT; i++) {
                idents.push_back(root->children[1]->tok->lexeme);
            }

            // get the assigned value or the default vale for the new variable
            if(i < root->children.size()) {
                val = eval(root->children[i], returnContext, memTable);
                checkForAllowedDtype(val, {dtype}, root->children[i]);
            } 
            else {
                val = AnyValue{{1}, defaultValueFor(dtype), dtype};
            }

            if(returnContext == nullptr) {
                // Store in global memory (we're not in a function)
                // TODO: global memory space will have to be allocated once the total number of global variables is known
            }
            else {
                // Store in the memory table (we're in a function)
                for(auto& i : idents) {
                    memTable->values.emplace_back(pair<string, AnyValue>{i, val});
                }
            }
            break;
        }

        case NODE_TYPE::VARIABLE_ASSIGN: {
            // get reference to target variable
            AnyValue targetRef = eval(root->children[0], returnContext, memTable);

            // =, +=, -=, etc
            TOK_TYPE assignOp = root->children[1]->tok->type;

            // evaluate new value
            AnyValue assignValue = eval(root->children[2], returnContext, memTable);

            // assign value
            if(isPrimitive(targetRef.type) && isPrimitive(assignValue.type)) {
                double value = extractNumValue(assignValue, root->children[2]);

                switch(assignOp) {
                    case TOK_TYPE::PLUS_EQUALS:
                        value = extractNumValue(targetRef, root->children[0]) + value;
                        break;
                    case TOK_TYPE::MINUS_EQUALS:
                        value =  extractNumValue(targetRef, root->children[0]) - value;
                        break;
                    case TOK_TYPE::STAR_EQUALS:
                        value = extractNumValue(targetRef, root->children[0]) * value;
                        break;
                    case TOK_TYPE::SLASH_EQUALS:
                        value = extractNumValue(targetRef, root->children[0]) / value;
                        break;
                    default:
                        break;
                }

                castAndAssign(targetRef, value);
            }
            else if(targetRef.type == assignValue.type) {
                if(assignOp != TOK_TYPE::EQUALS) {
                    if(targetRef.type == EVAL_RES_TYPE::String && assignOp == TOK_TYPE::PLUS_EQUALS) {
                        // append string
                        *(string*)targetRef.value.get() += *(string*)assignValue.value.get();
                    } else {
                        throwScribbleError(root, "Invalid operation for '" + data_type_to_string(targetRef.type) + "'", ERR_TYPE::INVALID_OPERATION);
                    }
                } else {
                    // TODO: cast non-primitive and assign
                }
            }
            else {
                throwScribbleError(root->children[2], "Variable type '" + data_type_to_string(targetRef.type) + "' does not match '" + data_type_to_string(assignValue.type) + "'", ERR_TYPE::INVALID_ASSIGNMENT);
            }

            break;
        }

        case NODE_TYPE::BUILT_IN_VAR_REFERENCE: {
            string ident = root->tok->lexeme;

            for(auto& var : BuiltInVariables) {
                if(var.first == ident) return var.second;
            }

            throwScribbleError(root, "Built in variable '" + ident + "' does not exist.", ERR_TYPE::INVALID_SYMBOL);
            break;
        }

        case NODE_TYPE::VARIABLE_REFERENCE: {
            string ident = root->children[0]->tok->lexeme;
            auto& memTablePtr = memTable;

            while(memTablePtr != nullptr) {
                for(auto& var : memTablePtr->values) {
                    if(var.first == ident) return var.second;
                }
                memTablePtr = memTablePtr->parent;
            }

            //TODO: search global memory

            throwScribbleError(root->children[0], "Variable '" + ident + "' not found.", ERR_TYPE::INVALID_SYMBOL);
            break;
        }

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

        case NODE_TYPE::FUNCTION_DEF: {
            string ident = root->children[0]->type == NODE_TYPE::IDENT ?
                            root->children[0]->tok->lexeme : root->children[1]->tok->lexeme;

            Functions.insert({ident, root});
            break;
        }

        case NODE_TYPE::FUNCTION_CALL: {
            string ident = root->children[0]->tok->lexeme;

            // attempt to find the function being called
            auto search = Functions.find(ident);
            if(search == Functions.end())
                throwScribbleError(root->children[0], "Function not defined", ERR_TYPE::INVALID_FUN_CALL);
            auto funPtr = search->second;

            // evaluate args
            vector<AnyValue> args;
            if(root->children.size() > 1) {
                for(auto &a : root->children[1]->children) {
                    args.emplace_back(eval(a, returnContext, memTable));
                }
            }

            // get parameter names and types
            vector<pair<EVAL_RES_TYPE, string>> params;
            for(auto &child : funPtr->children) {
                if(child->type == NODE_TYPE::PARAMETERS) {
                    for(size_t i=0; i<child->children.size()-1; i+=2) {
                        EVAL_RES_TYPE type = dtypeFromIdent(child->children[i]);
                        string name = child->children[i+1]->tok->lexeme;
                        params.push_back({type, name});
                    }

                    break;
                }
            }

            // create stack / scope
            auto funMemTable = make_shared<SymbolTable>();
            funMemTable->parent = nullptr;

            // check if args match params
            if(args.size() == params.size()) {
                for(size_t i=0; i<args.size(); i++) {
                    string name = params[i].second;

                    if(isPrimitive(params[i].first) && isPrimitive(args[i].type)) {
                        // convert the passed argument to a numerical value and then recast to target parameter type
                        double rawArg = extractNumValue(args[i], root);

                        // reassign type and automatically cast
                        args[i].type = params[i].first;
                        castAndAssign(args[i], rawArg);

                        funMemTable->values.push_back({
                            name,
                            args[i]
                        });
                    }
                    else if(!isPrimitive(params[i].first) && !isPrimitive(args[i].type)) {
                        if(params[i].first == args[i].type) {
                            // pass by reference
                            funMemTable->values.push_back({
                                name, 
                                args[i]
                            });
                        } else {
                            throwScribbleError(root->children[1], "Expected type '" + data_type_to_string(params[i].first) + "', got '" + data_type_to_string(args[i].type) + "'", ERR_TYPE::BAD_TYPE);
                        }
                    }
                    else {
                        throwScribbleError(root->children[1], "Expected type '" + data_type_to_string(params[i].first) + "', got '" + data_type_to_string(args[i].type) + "'", ERR_TYPE::BAD_TYPE);
                    }
                }
            } else {
              throwScribbleError(root->children[0], "Invalid number of arguments. Expected: " + to_string(params.size()) + ", got: " + to_string(args.size()), ERR_TYPE::INVALID_FUN_CALL);  
            }

            // evaluate function call
            auto funcReturnCtx = make_shared<AnyValue>();
            funcReturnCtx->type = EVAL_RES_TYPE::None;
            eval(funPtr->children[funPtr->children.size()-1], funcReturnCtx, funMemTable);

            // return value if value returned
            if(funcReturnCtx != nullptr && funcReturnCtx->type != EVAL_RES_TYPE::None && funcReturnCtx->type != EVAL_RES_TYPE::RETURN)
                return *funcReturnCtx;

            break;
        }

        case NODE_TYPE::RETURN_STATEMENT:
            if(root->children.size() > 0)
                *returnContext = eval(root->children[0], returnContext, memTable);
            else
                returnContext->type = EVAL_RES_TYPE::RETURN;
            break;

        case NODE_TYPE::EXP_ORL: {
            AnyValue one = eval(root->children[0], returnContext, memTable);
            AnyValue two = eval(root->children[1], returnContext, memTable);
            bool res = extractNumValue(one, root->children[0]) || extractNumValue(two, root->children[1]);

            return AnyValue{{1}, make_shared<bool>(res), EVAL_RES_TYPE::Bool};
        }

        case NODE_TYPE::EXP_ANDL: {
            AnyValue one = eval(root->children[0], returnContext, memTable);
            AnyValue two = eval(root->children[1], returnContext, memTable);
            bool res = extractNumValue(one, root->children[0]) && extractNumValue(two, root->children[1]);

            return AnyValue{{1}, make_shared<bool>(res), EVAL_RES_TYPE::Bool};
        }

        case NODE_TYPE::EXP_OR: {
            AnyValue one = eval(root->children[0], returnContext, memTable);
            AnyValue two = eval(root->children[1], returnContext, memTable);
            checkForAllowedDtype(one, {EVAL_RES_TYPE::Num}, root->children[0]);
            checkForAllowedDtype(two, {EVAL_RES_TYPE::Num}, root->children[1]);
            SCRIBBLE_NUM_REP res = *(SCRIBBLE_NUM_REP*)one.value.get() | *(SCRIBBLE_NUM_REP*)two.value.get();

            return AnyValue{{1}, make_shared<SCRIBBLE_NUM_REP>(res), EVAL_RES_TYPE::Num};
        }

        case NODE_TYPE::EXP_XOR: {
            AnyValue one = eval(root->children[0], returnContext, memTable);
            AnyValue two = eval(root->children[1], returnContext, memTable);
            checkForAllowedDtype(one, {EVAL_RES_TYPE::Num}, root->children[0]);
            checkForAllowedDtype(two, {EVAL_RES_TYPE::Num}, root->children[1]);
            SCRIBBLE_NUM_REP res = *(SCRIBBLE_NUM_REP*)one.value.get() ^ *(SCRIBBLE_NUM_REP*)two.value.get();

            return AnyValue{{1}, make_shared<SCRIBBLE_NUM_REP>(res), EVAL_RES_TYPE::Num};
        }

        case NODE_TYPE::EXP_AND: {
            AnyValue one = eval(root->children[0], returnContext, memTable);
            AnyValue two = eval(root->children[1], returnContext, memTable);
            checkForAllowedDtype(one, {EVAL_RES_TYPE::Num}, root->children[0]);
            checkForAllowedDtype(two, {EVAL_RES_TYPE::Num}, root->children[1]);
            SCRIBBLE_NUM_REP res = *(SCRIBBLE_NUM_REP*)one.value.get() & *(SCRIBBLE_NUM_REP*)two.value.get();

            return AnyValue{{1}, make_shared<SCRIBBLE_NUM_REP>(res), EVAL_RES_TYPE::Num};
        }

        case NODE_TYPE::EXP_EQ: {
            AnyValue one = eval(root->children[0], returnContext, memTable);
            AnyValue two = eval(root->children[1], returnContext, memTable);
            bool res;

            if(root->tok->type == TOK_TYPE::CMP_EQUALS)
                res = extractNumValue(one, root->children[0]) == extractNumValue(two, root->children[1]);
            else if(root->tok->type == TOK_TYPE::CMP_NOT_EQUALS)
                res = extractNumValue(one, root->children[0]) != extractNumValue(two, root->children[1]);
            else
                throwScribbleError(root, "EQ symbol not found", ERR_TYPE::INVALID_SYMBOL);

            return AnyValue{{1}, make_shared<bool>(res), EVAL_RES_TYPE::Bool};
        }

        case NODE_TYPE::EXP_CMP: {
            AnyValue one = eval(root->children[0], returnContext, memTable);
            AnyValue two = eval(root->children[1], returnContext, memTable);
            bool res;

            if(root->tok->type == TOK_TYPE::GREATER_THAN)
                res = extractNumValue(one, root->children[0]) > extractNumValue(two, root->children[1]);
            else if(root->tok->type == TOK_TYPE::GREATER_THAN_EQUAL)
                res = extractNumValue(one, root->children[0]) >= extractNumValue(two, root->children[1]);
            else if(root->tok->type == TOK_TYPE::LESS_THAN)
                res = extractNumValue(one, root->children[0]) < extractNumValue(two, root->children[1]);
            else if(root->tok->type == TOK_TYPE::LESS_THAN_EQUAL)
                res = extractNumValue(one, root->children[0]) <= extractNumValue(two, root->children[1]);
            else if(root->tok->type == TOK_TYPE::CMP_EQUALS)
                res = extractNumValue(one, root->children[0]) == extractNumValue(two, root->children[1]);
            else if(root->tok->type == TOK_TYPE::CMP_NOT_EQUALS)
                res = extractNumValue(one, root->children[0]) != extractNumValue(two, root->children[1]);
            else
                throwScribbleError(root, "CMP symbol not found", ERR_TYPE::INVALID_SYMBOL);

            return AnyValue{{1}, make_shared<bool>(res), EVAL_RES_TYPE::Bool};
        }

        case NODE_TYPE::EXP_SHFT: {
            AnyValue one = eval(root->children[0], returnContext, memTable);
            AnyValue two = eval(root->children[1], returnContext, memTable);
            checkForAllowedDtype(one, {EVAL_RES_TYPE::Num}, root->children[0]);
            checkForAllowedDtype(two, {EVAL_RES_TYPE::Num}, root->children[1]);
            SCRIBBLE_NUM_REP res;

            if(root->tok->type == TOK_TYPE::SHIFT_LEFT)
                res = *(SCRIBBLE_NUM_REP*)one.value.get() << *(SCRIBBLE_NUM_REP*)two.value.get();
            else
                res = *(SCRIBBLE_NUM_REP*)one.value.get() >> *(SCRIBBLE_NUM_REP*)two.value.get();

            return AnyValue{{1}, make_shared<SCRIBBLE_NUM_REP>(res), EVAL_RES_TYPE::Num};
        }

        case NODE_TYPE::EXP_ADD: {
            AnyValue one = eval(root->children[0], returnContext, memTable);
            AnyValue two = eval(root->children[1], returnContext, memTable);

            if(one.type == EVAL_RES_TYPE::String && one.type == two.type) {
                if(root->tok->type == TOK_TYPE::MINUS)
                    throwScribbleError(root, "Cannot subtract strings", ERR_TYPE::INVALID_OPERATION);

                string res = *(string*)one.value.get() + *(string*)two.value.get();
                return AnyValue{{1}, make_shared<string>(res), EVAL_RES_TYPE::String};
            }

            double res;
            if(root->tok->type == TOK_TYPE::PLUS)
                res = extractNumValue(one, root->children[0]) + extractNumValue(two, root->children[1]);
            else
                res = extractNumValue(one, root->children[0]) - extractNumValue(two, root->children[1]);

            auto casted = castNumValue(res, one.type, two.type);
            return AnyValue{{1}, casted.first, casted.second};
        }

        case NODE_TYPE::EXP_MULT: {
            AnyValue one = eval(root->children[0], returnContext, memTable);
            AnyValue two = eval(root->children[1], returnContext, memTable);
            double res;

            if(root->tok->lexeme == "*")
                res = extractNumValue(one, root->children[0]) * extractNumValue(two, root->children[1]);
            else
                res = extractNumValue(one, root->children[0]) / extractNumValue(two, root->children[1]);

            auto casted = castNumValue(res, one.type, two.type);
            return AnyValue{{1}, casted.first, casted.second};
        }

        case NODE_TYPE::EXP_POW: {
            AnyValue baseVal = eval(root->children[0], returnContext, memTable);
            AnyValue powVal = eval(root->children[1], returnContext, memTable);
            double res = pow(
                extractNumValue(baseVal, root->children[0]),
                extractNumValue(powVal, root->children[1])
            );

            auto casted = castNumValue(res, baseVal.type, powVal.type);
            return AnyValue{{1}, casted.first, casted.second};
        }

        case NODE_TYPE::EXP_NOT: {
            AnyValue val = eval(root->children[0], returnContext, memTable);
            double extracted = extractNumValue(val, root->children[0]);
            return AnyValue{{1}, make_shared<bool>(!extracted), EVAL_RES_TYPE::Bool};
        }

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


void checkForAllowedDtype(AnyValue val, vector<EVAL_RES_TYPE> dtypes, shared_ptr<AST_Node> &node) {
    for(EVAL_RES_TYPE &t : dtypes) {
        if(val.type == t) return;
    }

    throwScribbleError(node, "Invalid value for datatype", ERR_TYPE::INVALID_ASSIGNMENT);
}

// returns true if the passed type is a primitive type
bool isPrimitive(EVAL_RES_TYPE t) {
    return t == EVAL_RES_TYPE::Bool || t == EVAL_RES_TYPE::Float || t == EVAL_RES_TYPE::Num;
}

void checkSingleVal(AnyValue val, shared_ptr<AST_Node> &node) {
    if(val.dimension.size() != 1 || val.dimension[0] != 1)
        throwScribbleError(node, "Value is array", ERR_TYPE::BAD_TYPE);
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

pair<shared_ptr<void>, EVAL_RES_TYPE> castNumValue(double val, EVAL_RES_TYPE type1, EVAL_RES_TYPE type2) {
    if(type1 == type2 && type1 == EVAL_RES_TYPE::Num)
        return {make_shared<SCRIBBLE_NUM_REP>((SCRIBBLE_NUM_REP)val), EVAL_RES_TYPE::Num};
    return {make_shared<SCRIBBLE_FLOAT_REP>((SCRIBBLE_FLOAT_REP)val), EVAL_RES_TYPE::Float};
}

double extractNumValue(AnyValue &val, shared_ptr<AST_Node> &node) {
    switch(val.type) {
        case EVAL_RES_TYPE::None:
            throwScribbleError(node, "None", ERR_TYPE::BAD_TYPE);
        case EVAL_RES_TYPE::Num:
            return *(SCRIBBLE_NUM_REP*)val.value.get();
        case EVAL_RES_TYPE::Float:
            return *(SCRIBBLE_FLOAT_REP*)val.value.get();
        case EVAL_RES_TYPE::Bool:
            return *(bool*)val.value.get() ? 1 : 0;
        case EVAL_RES_TYPE::String:
            throwScribbleError(node, "String", ERR_TYPE::BAD_TYPE);
        case EVAL_RES_TYPE::Object:
            throwScribbleError(node, "Object", ERR_TYPE::BAD_TYPE);
        default:
            throwScribbleError(node, "Unknown", ERR_TYPE::BAD_TYPE);
    }

    return 0;
}

void castAndAssign(AnyValue &val, double newVal) {
    switch(val.type) {
         case EVAL_RES_TYPE::Num:
            val.value = make_shared<SCRIBBLE_NUM_REP>((SCRIBBLE_NUM_REP)newVal);
            break;
        case EVAL_RES_TYPE::Float:
            val.value = make_shared<SCRIBBLE_FLOAT_REP>((SCRIBBLE_FLOAT_REP)newVal);
            break;
        case EVAL_RES_TYPE::Bool:
            val.value = make_shared<bool>((bool)newVal);
            break;
        default:
            log("Interpreter -> castAndAssign: Unexpected data type"); // Technically a developer error, not a user error, no need to throw
            break;
    }
}

void throwScribbleError(shared_ptr<AST_Node> node, string message, ERR_TYPE type) {
    size_t l = 0, c = 0;

    if(node != nullptr && node->tok != nullptr) {
        l = node->tok->line;
        c = node->tok->start_col;
    }

    throw ScribbleErr {l, c, message, type};
}