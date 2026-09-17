cmake_minimum_required(VERSION 3.10)
project(MXLex VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(mxlex STATIC source/token.cpp source/scanner.cpp)
target_include_directories(mxlex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(test-lex main.cpp)
target_compile_options(mxlex PRIVATE
    -Wall
    -Wextra
    -pedantic
)

target_compile_options(test-lex PRIVATE
    -Wall
    -Wextra
    -pedantic
)
target_include_directories(test-lex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test-lex PRIVATE mxlex)


