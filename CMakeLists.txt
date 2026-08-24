cmake_minimum_required(VERSION 3.28)

PROJECT (scribble LANGUAGES CXX)

add_compile_options(-Wall -Werror) #use this for development, GET RID OF ALL WARNINGS BEFORE PUSHING
file( GLOB SOURCES "src/*.cpp" "src/utils/*.cpp" "src/runtime/*.cpp" )

include_directories( include )
add_executable( scribble ${SOURCES} )