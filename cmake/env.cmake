# Directories
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# Flags
IF (NOT WIN32)
    IF (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(CMAKE_CXX_FLAGS "--std=c++14 -g -arch arm64")
    ELSE ()
        set(CMAKE_CXX_FLAGS "--std=c++14 -g")
    ENDIF ()
ENDIF ()

# Packages
FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIRS})
LINK_DIRECTORIES(${OPENGL_LIBRARY_DIRS})
ADD_DEFINITIONS(${OPENGL_DEFINITIONS})

MESSAGE(STATUS "OpenGL: ${OPENGL_LIBRARIES}")
LIST(APPEND stdgl_libraries ${OPENGL_gl_LIBRARY})

if (APPLE)
	FIND_LIBRARY(COCOA_LIBRARY Cocoa REQUIRED)
	FIND_LIBRARY(IOKIT_LIBRARY IOKit REQUIRED)
	FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo REQUIRED)
	LIST(APPEND stdgl_libraries iconv ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${CoreVideo_LIBRARY})
	if (EXISTS /usr/local/opt/qt5)
		# Homebrew installs Qt5 (up to at least 5.9.1) in
		# /usr/local/qt5, ensure it can be found by CMake since
		# it is not in the default /usr/local prefix.
		list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
	endif()
endif(APPLE)
