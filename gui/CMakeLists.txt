cmake_minimum_required(VERSION 2.8.11)

project(dove-eye)

find_package(Qt5Widgets)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

file(GLOB SOURCES *.cc)
file(GLOB HEADERS *.h)
file(GLOB UI *.ui)

qt5_wrap_ui(UIC ${UI})

add_library(gui ${SOURCES} ${UIC})

target_link_libraries(gui Qt5::Widgets)



