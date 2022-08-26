
cmake_minimum_required(VERSION 3.23)
project(ipc)

add_executable(
    ipc
    src/main.cpp
    )

target_compile_features(
    ipc
    PRIVATE
    cxx_std_17
    )

find_package (Threads)

target_link_libraries (
    ipc
    PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    )

