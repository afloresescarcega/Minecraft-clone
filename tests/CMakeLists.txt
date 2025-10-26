# Test suite CMakeLists.txt

# Enable testing
enable_testing()

# Download and configure Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Bitmap tests - build independently without main project dependencies
add_executable(bitmap_test
  bitmap_test.cpp
  ${CMAKE_SOURCE_DIR}/lib/pmdreader/bitmap.cpp
)

# Include only what we need for bitmap tests
target_include_directories(bitmap_test PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/pmdreader
  ${CMAKE_SOURCE_DIR}/lib/utgraphicsutil
)

target_link_libraries(bitmap_test PRIVATE
  GTest::gtest_main
)

# Add test to CTest
include(GoogleTest)
gtest_discover_tests(bitmap_test)

# DebugGL tests - build with stubbed OpenGL/GLFW headers
add_executable(debuggl_test
  debuggl_test.cpp
  ${CMAKE_SOURCE_DIR}/lib/utgraphicsutil/debuggl.cc
)

target_include_directories(debuggl_test PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/utgraphicsutil
  ${CMAKE_SOURCE_DIR}/tests/stubs
)

target_link_libraries(debuggl_test PRIVATE
  GTest::gtest_main
)

gtest_discover_tests(debuggl_test)

# Code coverage configuration (for gcov/lcov)
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(bitmap_test PRIVATE --coverage)
    target_link_options(bitmap_test PRIVATE --coverage)
    target_compile_options(debuggl_test PRIVATE --coverage)
    target_link_options(debuggl_test PRIVATE --coverage)
  endif()
endif()
