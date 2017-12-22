cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(WiremockUI)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

set(WiremockUI_SRCS main.cpp mainwindow.cpp mainwindow.ui requestspage.cpp requestspage.ui settings.cpp wiremock.cpp)

add_executable(WiremockUI ${WiremockUI_SRCS})
target_link_libraries(WiremockUI Qt5::Widgets Qt5::Network)

