cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_LIBRARY_PATH ${QGLViewer_ROOT};${CMAKE_LIBRARY_PATH})

find_library(QGLViewer_LIB
	NAMES QGLViewer
	PATHS ${QGLViewer_ROOT}/QGLViewer)

find_library(gl_LIB
	NAMES GL)

find_path(QGLViewer_INCLUDE_DIR
	NAMES QGLViewer/qglviewer.h
	PATHS ${QGLViewer_ROOT})


find_package(Qt5OpenGL)
get_target_property(Qt5OpenGL_INCLUDE_DIRS Qt5::OpenGL INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(Qt5OpenGL_LIBS Qt5::OpenGL LOCATION)

find_package(Qt5Xml)
get_target_property(Qt5Xml_INCLUDE_DIRS Qt5::Xml INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(Qt5Xml_LIBS Qt5::Xml LOCATION)

set(QGLViewer_INCLUDE_DIRS ${QGLViewer_INCLUDE_DIR};${Qt5Xml_INCLUDE_DIRS};${Qt5OpenGL_INCLUDE_DIRS})
set(QGLViewer_LIBS ${QGLViewer_LIB};${Qt5Xml_LIBS};${Qt5OpenGL_LIBS};${gl_LIB})
