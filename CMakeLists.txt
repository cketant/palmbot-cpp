cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Palmbot)

option(PALMBOT_BUILD_TESTS "Build unit tests" OFF)

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