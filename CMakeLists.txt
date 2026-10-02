cmake_minimum_required(VERSION 3.31.6)

project(Palmbot)

add_executable(Main)
target_sources(Main
  PRIVATE
    src/config/pins.h

    src/hal/i2c_bus.h
    src/hal/i2c_device.h
    src/hal/linux_i2c_bus.h
    src/hal/linux_i2c_bus.cpp

    src/drivers/mpu6050.h
    src/drivers/mpu6050.cpp
    
    src/main.cpp
)

target_include_directories(Main
  PRIVATE
    src/
)
