#include "utils/helpers.hpp"
#include "runtime/templates.hpp"
#include "debug.hpp"

using namespace Interpreter;
using namespace std;

void Utils::checkForAllowedDtype(AnyValue val, vector<EVAL_RES_TYPE> dtypes, shared_ptr<AST_Node> &node) {
    for(EVAL_RES_TYPE &t : dtypes) {
        if(val.type == t) return;
    }

    throwScribbleError(node, "Invalid value for datatype", ERR_TYPE::INVALID_ASSIGNMENT);
}

// returns true if the passed type is a primitive type
bool Utils::isPrimitive(EVAL_RES_TYPE t) {
    return t == EVAL_RES_TYPE::Bool || t == EVAL_RES_TYPE::Float || t == EVAL_RES_TYPE::Num;
}

void Utils::checkSingleVal(AnyValue val, shared_ptr<AST_Node> &node) {
    if(val.dimension.size() != 1 || val.dimension[0] != 1)
        throwScribbleError(node, "Value is array", ERR_TYPE::BAD_TYPE);
}

shared_ptr<void> Utils::defaultValueFor(EVAL_RES_TYPE resType) {
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
            return make_shared<TemplateObjects::Object>();
        default:
            break; // TODO: custom values
    }

    return make_shared<int>(0);
}

// Make a duplicate of a variable
shared_ptr<void> Utils::duplicateValue(EVAL_RES_TYPE type, shared_ptr<void> val) {
    switch(type) {
        case EVAL_RES_TYPE::None:
            return make_shared<int>(0);
        case EVAL_RES_TYPE::Num:
            return make_shared<SCRIBBLE_NUM_REP>(*(SCRIBBLE_NUM_REP*)val.get());
        case EVAL_RES_TYPE::Float:
            return make_shared<SCRIBBLE_FLOAT_REP>(*(SCRIBBLE_FLOAT_REP*)val.get());
        case EVAL_RES_TYPE::Bool:
            return make_shared<bool>(*(bool*)val.get());
        case EVAL_RES_TYPE::String:
            return make_shared<string>(*(string*)val.get());
        case EVAL_RES_TYPE::Object: {
            TemplateObjects::Object clone((TemplateObjects::Object*)val.get());
            return make_shared<TemplateObjects::Object>(clone);
        }
        default:
            break; // TODO: custom values
    }

    return make_shared<int>(0);
}

// recursive method of creating a new <<matrix of variable size
shared_ptr<void> Utils::newArrayOfShape(vector<int> shape, EVAL_RES_TYPE dtype) {
    auto arr = make_shared<vector<AnyValue>>();
    auto start = shape.begin()+1;
    vector<int> sub(start, shape.end());

    for(int i=0; i<shape[0]; i++) {
        if(shape.size() == 1) {
            arr->push_back(AnyValue {
                {1},
                defaultValueFor(dtype),
                dtype
            });
        } else {
            arr->push_back(AnyValue {
                sub,
                newArrayOfShape(sub, dtype),
                dtype
            });
        }
    }

    return arr;
}

AnyValue Utils::valueFromArrayIndex(AnyValue arr, vector<int> index) {
    // bounds check
    if(index.size() == 0 || index[0] < 0 || index[0] >= arr.dimension[0])
        throwScribbleError(nullptr, "Array index out of bounds", ERR_TYPE::OOB);
    
    // I'm so sorry for this line
    // Getting the "index[0]" index from the array casted from a void pointer
    AnyValue res = (*(vector<AnyValue>*)arr.value.get()) [index[0]];

    if(index.size() == 1)
        return res;

    auto start = index.begin()+1;
    vector<int> sub(start, index.end());
    return valueFromArrayIndex(res, sub);
}

pair<shared_ptr<void>, EVAL_RES_TYPE> Utils::castNumValue(double val, EVAL_RES_TYPE type1, EVAL_RES_TYPE type2) {
    if(type1 == type2 && type1 == EVAL_RES_TYPE::Num)
        return {make_shared<SCRIBBLE_NUM_REP>((SCRIBBLE_NUM_REP)val), EVAL_RES_TYPE::Num};
    return {make_shared<SCRIBBLE_FLOAT_REP>((SCRIBBLE_FLOAT_REP)val), EVAL_RES_TYPE::Float};
}

double Utils::extractNumValue(AnyValue &val, shared_ptr<AST_Node> &node) {
    // Check if this is an array and if so, throw because we can't get a single value
    if(val.dimension.size() != 1 || val.dimension[0] != 1)
        throwScribbleError(node, "Expected value, got collection", ERR_TYPE::BAD_TYPE);

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

void Utils::castAndAssign(AnyValue &val, double newVal, bool inPlace) {
    switch(val.type) {
         case EVAL_RES_TYPE::Num:
            if(inPlace)
                *(SCRIBBLE_NUM_REP*)val.value.get() = (SCRIBBLE_NUM_REP)newVal;
            else
                val.value = make_shared<SCRIBBLE_NUM_REP>((SCRIBBLE_NUM_REP)newVal);
            break;
        case EVAL_RES_TYPE::Float:
            if(inPlace)
                *(SCRIBBLE_FLOAT_REP*)val.value.get() = (SCRIBBLE_FLOAT_REP)newVal;
            else
                val.value = make_shared<SCRIBBLE_FLOAT_REP>((SCRIBBLE_FLOAT_REP)newVal);
            break;
        case EVAL_RES_TYPE::Bool:
            if(inPlace)
                *(bool*)val.value.get() = (bool)newVal;
            else
                val.value = make_shared<bool>((bool)newVal);
            break;
        default:
            log("Interpreter -> castAndAssign: Unexpected data type"); // Technically a developer error, not a user error, no need to throw
            break;
    }
}

void Utils::indexVariable(AnyValue* val, AST_Node* idx, shared_ptr<vector<size_t>> ownedGlobalVariables, shared_ptr<SymbolTable> memTable) {
    vector<int> index;
    for(auto &indexNode : idx->children) {
        AnyValue dim = eval(indexNode, nullptr, ownedGlobalVariables, memTable);
        index.push_back(
            extractNumValue(dim, indexNode)
        );
    }

    *val = valueFromArrayIndex(*val, index);
}

void Utils::throwScribbleError(shared_ptr<AST_Node> node, string message, ERR_TYPE type, shared_ptr<ScribbleErr> parent) {
    size_t l = 0, c = 0;

    if(node != nullptr && node->tok != nullptr) {
        l = node->tok->line;
        c = node->tok->start_col;
    }

    throw ScribbleErr {l, c, message, type, parent};
}