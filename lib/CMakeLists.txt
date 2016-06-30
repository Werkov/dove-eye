cmake_minimum_required(VERSION 2.8)

project(dove-eye)

find_package(OpenCV REQUIRED)
find_package(OpenTLD)

file(GLOB SOURCES src/*.cc src/frame_iterator/*.cc)

add_library(dove-eye ${SOURCES})
target_link_libraries(dove-eye ${OpenCV_LIBS} ${OpenTLD_LIBS})

if(WIN32)
	target_link_libraries(dove-eye)
else()
	# TODO why it's not automatic with C++
	target_link_libraries(dove-eye pthread)
endif()


include_directories(./include)
include_directories(${OpenTLD_INCLUDE_DIRS})

if(WIN32)
        include_directories(${OpenCV_INCLUDE_DIRS})
endif()

