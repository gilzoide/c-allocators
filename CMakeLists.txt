cmake_minimum_required(VERSION 2.6)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_SOVERSION ${PROJECT_VERSION_MAJOR})

if(${CMAKE_VERSION} VERSION_GREATER 3.0)
	cmake_policy(SET CMP0048 NEW)
	project(c-allocators VERSION ${PROJECT_VERSION})
else()
	project(c-allocators)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -Wall")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -Os -DNDEBUG")
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

option(BUILD_SHARED_LIBS "Build as a shared library" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/.cmake")

add_subdirectory(src)

option(ENABLE_TESTS "Enable tests to be built and run with `make test`" OFF)
if(ENABLE_TESTS)
	find_package(Criterion REQUIRED)
	enable_testing()
	add_subdirectory(test)
endif()
