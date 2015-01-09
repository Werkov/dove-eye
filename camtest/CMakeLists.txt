cmake_minimum_required(VERSION 2.8)

project(CamTest)

find_package(OpenCV REQUIRED)

add_executable(camtest camtest.cpp )
target_link_libraries(camtest ${OpenCV_LIBS})

if(WIN32)
	include_directories(${OpenCV_INCLUDE_DIRS})
else()
	add_definitions("-std=c++11")
endif()

