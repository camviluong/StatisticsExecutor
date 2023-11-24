cmake_minimum_required(VERSION 3.0.0)
project(StatisticsExecutor VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# Linux
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	add_definitions(-D__UNIX__ -D__POSIX__ -DRAPIDJSON_HAS_STDSTRING)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CFLAGS} -g")

	message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

	add_executable(StatisticsExecutor  main.cpp)

	set(CPACK_PROJECT_NAME ${PROJECT_NAME})
	set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
	include(CPack)

# Windows
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_definitions(-D_WINDOWS
		-D_CONSOLE
		-DWIN32
		-DRAPIDJSON_HAS_STDSTRING)

    add_executable(StatisticsExecutor main.cpp)
endif()
