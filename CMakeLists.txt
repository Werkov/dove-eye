cmake_minimum_required(VERSION 2.8)

project(dove-eye)

if(WIN32)
	# TODO add warnings switch
else()
	add_definitions("-Wall -Wno-sign-compare")
endif()

add_subdirectory(lib)
add_subdirectory(tools/calibration)

