#include <memory>

#include "err.hpp"
#include "debug.hpp"
#include "runtime/built_in.hpp"

using namespace std;
using namespace Interpreter;

// built in functions prototypes
AnyValue BI_fun_print(vector<AnyValue>);

// helper prototypes
void assert_arg_types(string funName, vector<EVAL_RES_TYPE> types, vector<AnyValue> &args);

void BuiltIn::init(SymbolTableValues &memory, PreMadeFunctions &funcs) {
    memory.clear();
    memory.emplace_back("JOY_X", AnyValue{{1}, make_shared<SCRIBBLE_FLOAT_REP>(0), EVAL_RES_TYPE::Float});
    memory.emplace_back("JOY_Y", AnyValue{{1}, make_shared<SCRIBBLE_FLOAT_REP>(0), EVAL_RES_TYPE::Float});

    funcs.clear();
    funcs.insert({PRINT_FUNC_NAME, BI_fun_print});
}

void BuiltIn::update(SymbolTableValues &memory) {

}

// define built in functions
AnyValue BI_fun_print(vector<AnyValue> args) {
    assert_arg_types(PRINT_FUNC_NAME, {EVAL_RES_TYPE::String}, args);
    log(*(string*)args[0].value.get());
}

// define helper functions
void assert_arg_types(string funName, vector<EVAL_RES_TYPE> types, vector<AnyValue> &args) {
    if(types.size() != args.size())
        // position of where error occurred is appended by calling function in interpreter, for now line and column is just zero
        throw ScribbleErr{0,0, "Built in function '" + funName + "' expected " + to_string(types.size()) + " arguments, got " + to_string(args.size()), ERR_TYPE::INVALID_FUN_CALL};

    for(size_t i=0; i<types.size(); i++) {
        if(args[i].type != types[i]) {
            // construct a list of what args were actually expected
            string expectedTypes = "";
            for(size_t j=0; j<types.size(); j++) {
                expectedTypes += data_type_to_string(types[j]);
                if(j+1 < types.size())
                    expectedTypes += ", ";
            }

            // position of where error occurred is appended by calling function in interpreter, for now line and column is just zero
            throw ScribbleErr{0,0, "Invalid call to '" + funName + "': Expected arguments of types <" + expectedTypes + ">", ERR_TYPE::INVALID_FUN_CALL};
        }
    }
}