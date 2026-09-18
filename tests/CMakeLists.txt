cmake_minimum_required(VERSION 3.28)

PROJECT (scribble_tests LANGUAGES CXX)

add_compile_options(-Wall -Werror)

# compile tests
file( GLOB TESTS_SRC "*.cpp" "lexer/*.cpp" "parser/*.cpp" "interpreter/*.cpp" )
add_executable( scribble_tests ${TESTS_SRC} )

#set up include directories for the main program
include_directories( ../include )

#compile the main program
file( GLOB SCRIBBLE_SRC "../src/*.cpp" "../src/utils/*.cpp" "../src/runtime/*.cpp")
list(REMOVE_ITEM SCRIBBLE_SRC "../src/main.cpp")
add_library( scribble ${SCRIBBLE_SRC} )

#link the compiled main program into the test code
target_link_libraries(scribble_tests scribble)