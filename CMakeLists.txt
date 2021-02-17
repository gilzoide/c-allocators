cmake_minimum_required(VERSION 2.6...3.19)

project(c-allocators)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

option(ENABLE_TESTS "Enable tests to be built and run with `make test`" OFF)
if(ENABLE_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()
