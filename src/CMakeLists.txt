# PalmCore
add_library(PalmCore)
target_sources(PalmCore
  PRIVATE
    hal/linux_i2c_bus.cpp
    drivers/mpu6050.cpp
  PUBLIC
    config/pins.h

    hal/i2c_bus.h
    hal/i2c_device.h
    hal/linux_i2c_bus.h

    drivers/mpu6050.h
)
target_include_directories(PalmCore
  PUBLIC
    .
)