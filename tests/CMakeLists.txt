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
      drivers/mpu6050_test.cpp
      mocks/hal/i2c_bus.h
  )
  
  target_link_libraries(PalmTest 
    PRIVATE 
      GTest::gmock_main
      PalmCore
  )

  target_include_directories(PalmTest
    PRIVATE
      .
  )

  include(GoogleTest)
  gtest_discover_tests(PalmTest)
endif()