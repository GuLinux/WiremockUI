add_subdirectory(core)
add_subdirectory(ui)
set(WiremockUI_SRCS main.cpp)

add_executable(WiremockUI ${WiremockUI_SRCS})
target_link_libraries(WiremockUI core ui Qt5::Widgets Qt5::Network)

