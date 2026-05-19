cmake_minimum_required(VERSION 3.16)
project(QtClient)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(client WIN32
    main_gui.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    ../common/formats.hpp
    ../common/game_room.hpp
    ../common/game_room.cpp
)

target_link_libraries(client Qt6::Widgets)

target_include_directories(client PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/asio/include
    ${CMAKE_SOURCE_DIR}/libs
)

target_compile_definitions(client PRIVATE _CRT_SECURE_NO_WARNINGS)
