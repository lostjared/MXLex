cmake_minimum_required(VERSION 3.10)
project(MXLex VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(mxlex STATIC
    source/token.cpp
    source/scanner.cpp
)

add_library(MXLex::mxlex ALIAS mxlex)

target_include_directories(mxlex PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_options(mxlex PRIVATE
    -Wall
    -Wextra
    -pedantic
)

add_executable(test-lex main.cpp)

target_compile_options(test-lex PRIVATE
    -Wall
    -Wextra
    -pedantic
)

target_link_libraries(test-lex PRIVATE mxlex)

install(TARGETS mxlex
    EXPORT MXLexTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY MXLex/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/MXLex
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT MXLexTargets
    FILE MXLexTargets.cmake
    NAMESPACE MXLex::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MXLex
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MXLexConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MXLexConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MXLex
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MXLexConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MXLexConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MXLexConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MXLex
)
