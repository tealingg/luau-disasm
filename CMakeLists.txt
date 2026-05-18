cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(luau_disasm LANGUAGES CXX)

add_executable(luau-disasm)

add_subdirectory(luau EXCLUDE_FROM_ALL)

option(LUAU_BUILD_CLI "Build CLI" OFF)
option(LUAU_BUILD_TESTS "Build tests" OFF)
option(LUAU_BUILD_WEB "Build Web module" OFF)
option(LUAU_WERROR "Warnings as errors" OFF)
option(LUAU_STATIC_CRT "Link with the static CRT (/MT)" OFF)

include(sources.cmake)

target_link_libraries(luau-disasm PRIVATE Luau.Compiler Luau.VM)