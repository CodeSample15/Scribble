cmake_minimum_required(VERSION 3.28)

PROJECT (scribble LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_compile_options(-O3 -Wall -Werror) # use Wall, Werror for development, GET RID OF ALL WARNINGS BEFORE PUSHING
file( GLOB SOURCES "src/*.cpp" "src/utils/*.cpp" "src/runtime/*.cpp" )

include_directories( include )
add_executable( scribble ${SOURCES} )
target_link_libraries( scribble PRIVATE SFML::Graphics )