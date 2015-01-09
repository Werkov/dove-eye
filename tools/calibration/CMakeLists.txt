cmake_minimum_required(VERSION 2.8)

project(calibration)

find_package(OpenCV REQUIRED)

add_executable(calibration main.cc )
target_link_libraries(calibration ${OpenCV_LIBS})


include_directories(${CMAKE_SOURCE_DIR}/lib/include)

if(WIN32)
        include_directories(${OpenCV_INCLUDE_DIRS})
else()
        add_definitions("-std=c++11")
endif()
