cmake_minimum_required(VERSION 2.8.11)

project(dove-eye)

find_package(OpenCV)
find_package(QGLViewer)
find_package(Qt5Widgets)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

file(GLOB SOURCES 
	*.cc
	gui/*.cc
	io/*.cc
	widgets/*.cc)
file(GLOB HEADERS
	*.h
	io/*.h
	gui/*.h
	widgets/*.h)
file(GLOB UI
	gui/*.ui
	widgets/*.ui)

qt5_wrap_ui(UIC ${UI})

add_library(gui ${SOURCES} ${UIC})

include_directories(${CMAKE_SOURCE_DIR}/app)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${QGLViewer_INCLUDE_DIRS})

add_definitions("-DHAVE_GUI")

target_link_libraries(gui
	Qt5::Widgets
	${OpenCV_LIBS}
	${QGLViewer_LIBS}
	dove-eye)



