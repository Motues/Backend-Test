cmake_minimum_required(VERSION 3.28)

project(Utils.JSON)

AddCXXModule(
    NAME "JSON"
    NAMESPACE "Utils"
    CXX_STANDARD "26"
    HEADER_DIR "include"
    SOURCE_DIR "src"
    TESTER_DIR "test"
)
