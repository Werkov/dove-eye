cmake_minimum_required(VERSION 2.8.11)

project(dove-eye)

find_package(Qt5Widgets)

add_executable(dove-eye_exec main.cc)
set_target_properties(dove-eye_exec PROPERTIES OUTPUT_NAME dove-eye)
target_link_libraries(dove-eye_exec dove-eye gui Qt5::Widgets)


include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_SOURCE_DIR}/gui)

add_definitions("-DHAVE_GUI")

if(WIN32)
	#TODO include path to Qt libraries
endif()
