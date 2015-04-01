cmake_minimum_required(VERSION 2.8)

project(dove-eye)
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(WIN32)
	# TODO add warnings switch
else()
	add_definitions("-std=c++11 -Wall -Wno-sign-compare")
endif()

add_subdirectory(app)
add_subdirectory(lib)
#add_subdirectory(tools/calibration)
add_subdirectory(tools/dove_eye)

