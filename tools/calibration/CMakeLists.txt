cmake_minimum_required(VERSION 2.8)

project(dove-eye)

find_package(OpenCV REQUIRED)

add_executable(calibration main.cc)
target_link_libraries(calibration dove-eye)


include_directories(${CMAKE_SOURCE_DIR}/lib/include)

if(WIN32)
        include_directories(${OpenCV_INCLUDE_DIRS})
endif()
