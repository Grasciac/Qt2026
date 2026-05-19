cmake_minimum_required(VERSION 3.16)
project(ConsoleServer)

add_executable(server main.cpp)

target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/asio/include
    ${CMAKE_SOURCE_DIR}/libs
)

target_compile_definitions(server PRIVATE _CRT_SECURE_NO_WARNINGS)
