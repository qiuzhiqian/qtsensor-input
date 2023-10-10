cmake_minimum_required(VERSION 3.10)
project(qtsensor_input)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(PkgConfig REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Sensors)
include_directories(./)

add_library(${PROJECT_NAME} SHARED main.cpp input_accelerometer.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Sensors)

