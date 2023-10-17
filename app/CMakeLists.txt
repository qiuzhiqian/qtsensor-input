cmake_minimum_required(VERSION 3.10)
project(sensor-app)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(PkgConfig REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Sensors DBus)

pkg_check_modules(qgsettings REQUIRED IMPORTED_TARGET gsettings-qt)
include_directories(./)

add_executable(${PROJECT_NAME} main.cpp accelerometer.cpp light.cpp brightness.cpp utils.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Sensors Qt5::DBus PkgConfig::qgsettings)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "bin"
)
install(FILES sensor-daemon.service
        DESTINATION lib/systemd/user)
