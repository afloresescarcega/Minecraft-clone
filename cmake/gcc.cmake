IF (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    IF (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.2.0")
        IF (EXISTS /lusr/opt/gcc-5.2.0/bin/g++)
            SET(CMAKE_CXX_COMPILER /lusr/opt/gcc-5.2.0/bin/g++)
            SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,/lusr/opt/gcc-5.2.0/lib64/")
        ENDIF ()
    ENDIF ()
    IF (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always -arch arm64")
    ELSE ()
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
    ENDIF ()
ENDIF ()

MESSAGE(STATUS "USING C++ Compiler ${CMAKE_CXX_COMPILER}")
