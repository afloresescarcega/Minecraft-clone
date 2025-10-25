CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

IF (WIN32)
	# VCPKG
	if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
		set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
	endif()
ENDIF ()

project(GLSL)

# Option to build tests (set before including cmake files)
option(BUILD_TESTS "Build the test suite" ON)

# Only include cmake dependencies if not building tests only
IF(NOT BUILD_TESTS_ONLY)
	FILE(GLOB cmakes ${CMAKE_SOURCE_DIR}/cmake/*.cmake)
	FOREACH(cmake ${cmakes})
		INCLUDE(${cmake})
	ENDFOREACH(cmake)

	MESSAGE(STATUS "stdgl: ${stdgl_libraries}")

	ADD_SUBDIRECTORY(src)
ENDIF()

# Add test subdirectory
IF(BUILD_TESTS OR BUILD_TESTS_ONLY)
	ADD_SUBDIRECTORY(tests)
ENDIF()

IF (EXISTS ${CMAKE_SOURCE_DIR}/sln/CMakeLists.txt)
	ADD_SUBDIRECTORY(sln)
ENDIF()
