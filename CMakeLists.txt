cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Palmbot)

option(PALMBOT_BUILD_TESTS "Build unit tests" OFF)

# PalmCore
add_library(PalmCore)
target_sources(PalmCore
  PRIVATE
    src/hal/linux_i2c_bus.cpp
    src/drivers/mpu6050.cpp
  PUBLIC
    src/config/pins.h

    src/hal/i2c_bus.h
    src/hal/i2c_device.h
    src/hal/linux_i2c_bus.h

    src/drivers/mpu6050.h
)
target_include_directories(PalmCore
  PUBLIC
    src/
)

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

# PalmTest
if(PALMBOT_BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  add_executable(PalmTest)
  target_sources(PalmTest
    PRIVATE
      tests/drivers/mpu6050_test.cpp
      tests/mocks/hal/i2c_bus.h

  )
  target_link_libraries(PalmTest 
    PRIVATE 
      GTest::gtest_main
      GTest::gmock_main
      PalmCore
  )
  target_include_directories(PalmTest
    PRIVATE
      tests/
      src/
  )

  include(GoogleTest)
  gtest_discover_tests(PalmTest)
endif()
