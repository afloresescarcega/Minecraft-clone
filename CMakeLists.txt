CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

IF (WIN32)
	# VCPKG
	if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
		set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
	endif()
ENDIF ()

project(GLSL)

# Set up coverage build type for CLion
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} --coverage -fprofile-arcs -ftest-coverage" CACHE STRING "Flags used by the C++ compiler during coverage builds." FORCE)
set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_DEBUG} --coverage -fprofile-arcs -ftest-coverage" CACHE STRING "Flags used by the C compiler during coverage builds." FORCE)
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage" CACHE STRING "Flags used for linking binaries during coverage builds." FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} --coverage" CACHE STRING "Flags used by the shared libraries linker during coverage builds." FORCE)
mark_as_advanced(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE
)

# Update the list of valid build types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel;Coverage" CACHE STRING "Available build types" FORCE)

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
