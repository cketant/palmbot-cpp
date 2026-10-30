# PalmCore
add_library(PalmCore)
target_sources(PalmCore
  PRIVATE
    hal/linux_i2c_bus.cpp

    drivers/mpu6050.cpp
    drivers/vl53l0x.cpp
    drivers/drv8833.cpp
    
    lib/conversion_math.cpp

    models/motor_gpiod.cpp
  PUBLIC
    config/pins.h

    hal/i2c_bus.h
    hal/i2c_device.h
    hal/linux_i2c_bus.h

    drivers/mpu6050.h
    drivers/vl53l0x.h
    drivers/drv8833.h

    lib/conversion_math.h
    lib/SPSCQueue.h
    
    models/measurement.h
    models/motor_gpiod.h
)
target_include_directories(PalmCore
  PUBLIC
    .
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED IMPORTED_TARGET libgpiodcxx)
target_link_libraries(PalmCore PUBLIC PkgConfig::GPIOD)