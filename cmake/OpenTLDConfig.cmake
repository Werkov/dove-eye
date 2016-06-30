cmake_minimum_required(VERSION 2.8.11)

# TODO Windows variant is not tested!

set(CMAKE_LIBRARY_PATH ${OpenTLD_ROOT};${CMAKE_LIBRARY_PATH})

if(WIN32)
	find_library(OpenTLD_LIB
		NAMES OpenTLD
		PATHS ${OpenTLD_ROOT}/OpenTLD/release)
else()
	find_library(OpenTLD_LIB
		NAMES opentld
		PATHS ${OpenTLD_ROOT}/lib)
	find_library(cvblobs_LIB
		NAMES cvblobs
		PATHS ${OpenTLD_ROOT}/lib)
endif()


find_path(OpenTLD_INCLUDE_DIR
	NAMES tld/TLD.h
	PATHS ${OpenTLD_ROOT}/include)


set(OpenTLD_INCLUDE_DIRS ${OpenTLD_INCLUDE_DIR})
set(OpenTLD_LIBS ${OpenTLD_LIB};${cvblobs_LIB})
