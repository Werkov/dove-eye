cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_LIBRARY_PATH ${QGLViewer_ROOT};${CMAKE_LIBRARY_PATH})

if(WIN32)
	find_library(QGLViewer_LIB
		NAMES QGLViewer2
		PATHS ${QGLViewer_ROOT}/QGLViewer/release)
else()
	find_library(QGLViewer_LIB
		NAMES QGLViewer-qt5 QGLViewer
		PATHS ${QGLViewer_ROOT}/QGLViewer)
endif()

if(WIN32)
	find_library(gl_LIB
		NAMES opengl32
		PATHS "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib/x64")
else()
	find_library(gl_LIB
		NAMES GL)
endif()

find_path(QGLViewer_INCLUDE_DIR
	NAMES QGLViewer/qglviewer.h
	PATHS ${QGLViewer_ROOT})

if(NOT CMAKE_BUILD_TYPE)
	set(build_type "RELEASE")
else()
	set(build_type "${CMAKE_BUILD_TYPE}")
endif()

find_package(Qt5OpenGL)
if(NOT Qt5OpenGL_INCLUDE_DIRS)
	get_target_property(Qt5OpenGL_INCLUDE_DIRS Qt5::OpenGL INTERFACE_INCLUDE_DIRECTORIES)
endif()

if(WIN32)
	set(varname "IMPORTED_IMPLIB_${build_type}")
	get_target_property(Qt5OpenGL_LIBS Qt5::OpenGL "${varname}")
else()
	get_target_property(Qt5OpenGL_LIBS Qt5::OpenGL LOCATION)
endif()

find_package(Qt5Xml)
if(NOT Qt5Xml_INCLUDE_DIRS)
	get_target_property(Qt5Xml_INCLUDE_DIRS Qt5::Xml INTERFACE_INCLUDE_DIRECTORIES)
endif()

if(WIN32)
	set(varname "IMPORTED_IMPLIB_${build_type}")
	get_target_property(Qt5Xml_LIBS Qt5::Xml "${varname}")
	message("v: ${varname}, b: ${build_type}, ${Qt5Xml_LIBS}.")
else()
	get_target_property(Qt5Xml_LIBS Qt5::Xml LOCATION)
endif()

set(QGLViewer_INCLUDE_DIRS ${QGLViewer_INCLUDE_DIR};${Qt5Xml_INCLUDE_DIRS};${Qt5OpenGL_INCLUDE_DIRS})
set(QGLViewer_LIBS ${QGLViewer_LIB};${Qt5Xml_LIBS};${Qt5OpenGL_LIBS};${gl_LIB})
