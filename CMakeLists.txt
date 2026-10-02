cmake_minimum_required(VERSION 3.31.6)

project(Palmbot)

add_executable(Main)
target_sources(Main
  PRIVATE
    src/config/pins.h
    src/drivers/i2c_bus.h
    src/drivers/linux_i2c_bus.h
    src/main.cpp
    src/drivers/linux_i2c_bus.cpp
)

target_include_directories(Main
  PRIVATE
    src/
)
