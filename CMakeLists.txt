cmake_minimum_required(VERSION 3.28)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Cango)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()


project(Backend)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Building on Windows")
    # 添加SQLite3
    set(SQLite3_INCLUDE_DIR D:/Soft/sqlite/include)
    set(SQLite3_LIBRARY D:/Soft/sqlite/lib)
    include_directories(${SQLite3_INCLUDE_DIR})
    link_directories(${SQLite3_LIBRARY})
    set(SQLite3_LINK "SQLite3")
    # include(D:/Soft/vcpkg/scripts/buildsystems/vcpkg.cmake)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Building on Linux")
    set(SQLite3_LINK "SQLite::SQLite3")
    # include(/home/motues/vcpkg/scripts/buildsystems/vcpkg.cmake)
else()
    message(FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#find_package(Boost REQUIRED COMPONENTS system)
#find_package(spdlog REQUIRED)
#find_package(nlohmann_json CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)


AddModulesInDirectory(
    DIRECTORY "CommonUtils"
    NAMES
        "Log"
        "MessageQueue"
        "ThreadPool"
        "JSON"
        "DataBase"
)


AddModulesInDirectory(
    DIRECTORY "Modules"
    NAMES
)

