cmake_minimum_required(VERSION 2.8)

project(CamTest)

add_definitions("-std=c++11")

find_package(OpenCV REQUIRED)

add_executable(camtest camtest.cpp )
target_link_libraries(camtest ${OpenCV_LIBS})
