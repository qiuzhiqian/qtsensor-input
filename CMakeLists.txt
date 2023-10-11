cmake_minimum_required(VERSION 3.10)
project(sensor)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif()

include(GNUInstallDirs)
add_subdirectory(app)
add_subdirectory(plugin)