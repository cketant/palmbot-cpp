#include <iostream>
#include "hal/linux_i2c_bus.h"
#include "drivers/mpu6050.h"

int main() {
  LinuxI2CBus bus("/dev/i2c-1");
  MPU6050 mpu6050(bus);


  return 0;
}
