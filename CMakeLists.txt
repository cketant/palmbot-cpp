cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Palmbot)

# Default to Debug (single-config generators) unless the user specifies one
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

############ Building and Running Tests ############
# cmake -S . -B build_tests -DPALMBOT_BUILD_TESTS=ON
# cmake --build build_tests
# ctest --test-dir build_tests --output-on-failure
####################################################

############ Building and Running clang-tidy ############
#  cmake -S . -B build_clang_tidy -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
#  run-clang-tidy -p build_clang_tidy
# clang-tidy -p build_clang_tidy <INDIVIDUAL_FILE> (Run against a single file)
# clang-tidy -p build_clang_tidy --fix <INDIVIDUAL_FILE> (Apply fixes against a single file)
####################################################

option(PALMBOT_BUILD_TESTS "Build unit tests" OFF)

# Must be in the top-level CMakeLists so ctest finds the tests from the build root
if(PALMBOT_BUILD_TESTS)
  enable_testing()
endif()

add_subdirectory(tests)
add_subdirectory(src)

# Main
add_executable(Main)
target_sources(Main
  PRIVATE
    src/main.cpp
)
target_link_libraries(Main
  PRIVATE
    PalmCore
)