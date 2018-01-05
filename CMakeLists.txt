cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(WiremockUI)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} src)
add_subdirectory(src)

#include(packaging.cmake)
