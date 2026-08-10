# Scribble

Make really simple games through a language that's built for that very purpose. 

## Purpose and goals

Scribble's aim is to be similar to its precursor [Squiggly](https://github.com/CodeSample15/Squiggly) while being faster, more robust, and easier to develop. 

There are countless ways to develop video games and graphical applications. However, there's a few issues I have with modern methods that I aim to fix with Scribble:

 - Most languages require you to find libraries to create a graphical interface and input manager for their application. This can lead to version issues, annoying linker errors, and confusion for first time developers.
    - Scribble comes with built in features to handle this, making it so that the user can immediately focus on what goes on the screen, not how it does so.
 - Distributing applications
    - Scribble will aim to run code on as many platforms as possible... including web. This means that users can write code for one device and safely share their code with others.
 - Security issues
    - Scribble has no way to interface directly with local files, make unsafe system calls, or anything similar. This means that users can execute other's Scribble files immediately after downloading without fear of accidentally running malware.
- Fun and education
    - Modern day programming education is very focused around using LLMs to write code. In my opinion, this takes the joy out of learning to code, especially for new developers. This language aims to provide a simple syntax that is both fun to use and teaches new developers how to think like a programmer, without having to rely on prompting.
    - The idea isn't to develop the next hackathon-winning app with this language, but instead just focus on making something cool for others to enjoy, while learning to think programmatically along the way.

## Progress

### Lexer (version 1):

- [X] Lex all tokens needed for parser
- [X] Comprehensive error reporting
- [X] Tests
- [ ] Edge case handling (invalid float literals?)

### Parser (version 1):

- [X] Program
- [X] import_statement
- [X] function_def
- [X] function_modifier
- [X] function_call
- [X] parameters
- [X] arguments
- [X] body
- [X] branch
- [X] variable_def
- [X] variable_reference
- [X] variable_assign
- [X] vartype
- [X] core_function
- [X] start_func
- [X] update_func
- [X] expression
- [X] chained_identifier

More abstract todo list:

- [X] General parser architecture
- [X] Node type enum
- [X] Fully functional AST setup
- [ ] Fix error reporting to be more useful (say what segment was expected, not just what token)
- [ ] Parser tests
- [ ] AST tests

### Interpreter:

*Not started*

## Language Grammar
```EBNF
program = {import_statement} , {core_function | function_def | variable_def}

import_statement    = 'use' , chained_identifier , 'as' , chained_identifier

core_function       = start_func | update_func
start_func          = ':START:{' , body , '}'
update_func         = ':UPDATE:{' , body , '}'


# Function declarations:
VALID_FUNCTION_MODIFIER = 'thread' | 'quiet' |  # room to grow with more function modifiers

function_def        = [function_modifier] , 'fun' , identifier , '(' , [parameters] , ')' , '{' , body , '}'

function_modifier   = '[' , VALID_FUNCTION_MODIFIER , {',' , VALID_FUNCTION_MODIFIER} , ']'

# function_call
function_call       =  chained_identifier , '(' , [arguments] , ')'

parameters          = VARTYPE , identifier , {',' , VARTYPE , identifier}
arguments           = expression , {',' , expression}

return_statement    = 'return' , expression

# Function bodies:
body                = {(variable_def 
                    | variable_assign 
                    | branch 
                    | function_call
                    | loop
                    | return_statement) , [';']}

# variables
variable_def        = VARTYPE , identifier , {',' , identifier} , ['=' , expression]
variable_assign     = variable_reference , ASSIGN_OP , expression

variable_reference  = normal_var_ref
                    | built_in_var_ref
                    
normal_var_ref      = chained_identifier , [arr_index]
built_in_var_ref    = '$' , identifier

arr_index           = '[' , expression , {',' , expression} , ']'

ASSIGN_OP           = '='
                    | '+='
                    | '-='
                    | '/='
                    | '*='
INCR_DECR_OP        = '--' | '++'

VARTYPE             = ('num' | 'float' | 'string' | CLASS_NAME) , [arr_index]

# branches
branch              = branch_if ,  {branch_if_else} , [branch_else]

branch_if           = 'if' , expression , '{' , body , '}'
branch_if_else      = 'if else' , expression , '{' , body , '}'
branch_else         = 'else' , '{' , body , '}'

# loops
loop                = while_loop | repeat_loop

while_loop          = 'while' , expression , '{' , body , '}'
repeat_loop         = 'repeat' , expression , '{' , body , '}'

chained_identifier  = {(function_call | identifier) , '.'} , (function_call | identifier)

# mathematical and boolean expressions
expression          = exp_orl
exp_orl             = exp_andl , { '||' , exp_andl }
exp_andl            = exp_or , { '&&' , exp_or }
exp_or              = exp_xor , { '|' , exp_xor }
exp_xor             = exp_and , { '^' , exp_and }
exp_and             = exp_eq , { '&' , exp_eq }
exp_eq              = exp_cmp , { ('==' | '!=') , exp_cmp }
exp_cmp             = exp_shft , { ('<' | '<=' | '>' | '>=') , exp_shft }
exp_shft            = exp_add , { ('<<' , '>>') , exp_add }
exp_add             = exp_mult , { ('+' | '-') , exp_mult }
exp_mult            = exp_pow , { ('*' | '/' | '%') , exp_pow }
exp_pow             = exp_not , [ '**' , exp_pow ]
exp_not             = ['!'] , exp_primary
exp_primary         = variable_reference
                    | literal
                    | '(' , expression , ')'
                    | function_call
```

NB: Grammar assumes whitespace has been stripped from the output of the lexer 

## Precedence Table

Derived from C, but stripped down a lot

| Precedence  | Operator / Constructs                                               | Associativity |
| ----------- | ------------------------------------------------------------------- | ------------- |
| 1 (highest) | Primary: variable_reference, literal, ( expression ), function_call | N/A           |
| 2           | `!`                                                                 | Right-to-left |
| 3           | `**`                                                                | Right-to-left |
| 4           | `*`, `/`, `%`                                                       | Left-to-right |
| 5           | `+`, `-`                                                            | Left-to-right |
| 6           | `<<`, `>>`                                                          | Left-to-right |
| 7           | `>`, `>=`, `<`, `<=`                                                | Left-to-right |
| 8           | `==`, `!=`                                                          | Left-to-right |
| 9           | `&`                                                                 | Left-to-right |
| 10          | `^`                                                                 | Left-to-right |
| 11          | Bitwise OR                                                          | Left-to-right |
| 12          | Logical AND                                                         | Left-to-right |
| 13          | Logical OR                                                          | Left-to-right |

## Example

<!--> Using Rust as the language type because it has the closest syntax highlighting <-->
```rust
use my_library as lib
use folder/fancy_graphics as graph

num count = 0;
num lives = 5       # optional semicolons
OBJECT[5, 4] grid;  # arrays are fixed-size C# style dynamically allocated arrays

:START: {
    lives += 1
}

:UPDATE: {
    if count > 4 {
        count = 0
        lives--
    }
}

[thread]
fun coroutine() {
    # can run parallel to main program
    while(true) {
        count++
    }
}
```