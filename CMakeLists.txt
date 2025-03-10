cmake_minimum_required(VERSION 3.28)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Cango)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

project(Backend)

#include(/home/motues/vcpkg/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#find_package(Boost REQUIRED COMPONENTS system filesystem json program_options)

AddModulesInDirectory(
    DIRECTORY "Modules"
    NAMES
        "Log"
        "BasicType"
        "WebUtils"

)

