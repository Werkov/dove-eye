cmake_minimum_required(VERSION 2.8.11)

project(dove-eye)

find_package(Qt5Widgets)

add_executable(dove_eye main.cc)
target_link_libraries(dove_eye gui Qt5::Widgets)


include_directories(${CMAKE_SOURCE_DIR}/gui)

if(WIN32)
	#TODO include path to Qt libraries
endif()
