cmake_minimum_required(VERSION 3.28)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Cango)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()


project(Backend)

#if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
#    message(STATUS "Building on Windows")
#    include(D:/Soft/vcpkg/scripts/buildsystems/vcpkg.cmake)
#elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
#    message(STATUS "Building on Linux")
#    include(/home/motues/vcpkg/scripts/buildsystems/vcpkg.cmake)
#else()
#    message(FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")
#endif()



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#find_package(Boost REQUIRED COMPONENTS system)
#find_package(spdlog REQUIRED)
#find_package(nlohmann_json CONFIG REQUIRED)

AddModulesInDirectory(
    DIRECTORY "CommonUtils"
    NAMES
        "ThreadPool"
        "JSON"
        "Log"
        "MessageQueue"
)


AddModulesInDirectory(
    DIRECTORY "Modules"
    NAMES
)

