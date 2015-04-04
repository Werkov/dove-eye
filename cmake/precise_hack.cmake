# Use API of old (without target properties) Qt CMake scripts
macro(setup_qt_module MODULE)
	include_directories(${${MODULE}_INCLUDE_DIRS})
	add_definitions(${${MODULE}_DEFINITIONS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${MODULE}_EXECUTABLE_COMPILE_FLAGS}")
endmacro()

setup_qt_module("Qt5Core")
setup_qt_module("Qt5Gui")
setup_qt_module("Qt5OpenGL")
setup_qt_module("Qt5Widgets")
setup_qt_module("Qt5Xml")
