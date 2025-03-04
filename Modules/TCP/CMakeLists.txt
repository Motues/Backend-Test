cmake_minimum_required(VERSION 3.28)

project(Backend.TCP)

AddCXXModule(
    NAME "TCP"
    NAMESPACE "Backend"
    CXX_STANDARD "26"
    HEADER_DIR "include"
    SOURCE_DIR "src"
    TESTER_DIR "test"
)
