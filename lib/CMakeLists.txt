cmake_minimum_required(VERSION 2.8)

project(dove-eye)

find_package(OpenCV REQUIRED)

file(GLOB SOURCES src/*.cc)

add_library(dove-eye ${SOURCES})
target_link_libraries(dove-eye ${OpenCV_LIBS})


include_directories(./include)

if(WIN32)
        include_directories(${OpenCV_INCLUDE_DIRS})
endif()
