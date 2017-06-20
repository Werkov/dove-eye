cmake_minimum_required(VERSION 2.8)

project(dove-eye)
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#
# Compile time configuration
#
include(CMakeDependentOption)

set(CONFIG_MAX_ARITY 3)

option(CONFIG_DEBUG_HIGHGUI "Use OpenCV highgui for debugging outputs" off)

# OpenCV highgui doesn't work well with multiple threads, therefore they must
# be disabled
cmake_dependent_option(
	CONFIG_SINGLE_THREADED "Do not create new threads for application logic"
	on "CONFIG_DEBUG_HIGHGUI" off)

configure_file(cmake/config.h.cmake config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#
# Common settings
#
if(WIN32)
	# TODO add warnings switch
else()
	add_definitions("-std=c++14 -Wall -Wno-sign-compare")
endif()


#
# Subdirectories
#
add_subdirectory(app)
add_subdirectory(lib)
#add_subdirectory(tools/calibration)
add_subdirectory(tools/dove_eye)

