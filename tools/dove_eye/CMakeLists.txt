cmake_minimum_required(VERSION 2.8.11)

project(dove-eye)

find_package(OpenCV REQUIRED)
find_package(Qt5Widgets)

add_executable(dove-eye_exec main.cc)
target_link_libraries(dove-eye_exec dove-eye gui Qt5::Widgets)

set_target_properties(dove-eye_exec PROPERTIES OUTPUT_NAME dove-eye)
set_target_properties(dove-eye_exec PROPERTIES
	CXX_STANDARD ${CXX_STANDARD})

include_directories(${CMAKE_SOURCE_DIR}/app)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)

include(${CMAKE_SOURCE_DIR}/cmake/precise_hack.cmake)

add_definitions("-DHAVE_GUI")

if(WIN32)
	include_directories(${OpenCV_INCLUDE_DIRS})
endif()

install(TARGETS dove-eye_exec
	DESTINATION bin)
