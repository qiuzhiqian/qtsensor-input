cmake_minimum_required(VERSION 3.10)
project(sensor-app)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(PkgConfig REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Sensors)
include_directories(./)

add_executable(${PROJECT_NAME} main.cpp manager.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Sensors)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "bin"
)
