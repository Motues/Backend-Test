cmake_minimum_required(VERSION 3.28)

project(Utils.MessageQueue)

AddCXXModule(
    NAME "MessageQueue"
    NAMESPACE "Utils"
    CXX_STANDARD "26"
    HEADER_DIR "include"
    SOURCE_DIR "src"
    TESTER_DIR "test"
    LINKS
)
