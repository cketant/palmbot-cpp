#include <iostream>
#include <chrono>
#include "hal/linux_i2c_bus.h"
#include "drivers/mpu6050.h"

int main() {
  try {
    LinuxI2CBus bus("/dev/i2c-1");
    MPU6050 mpu6050(bus);
    while (true) {
      std::optional<Measurement> m = mpu6050.measurement();
      if (m) {
        std::cout << "We have a value" << std::endl;
      } else {
        std::cout << "We DON'T have a value" << std::endl;
      }
    };
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}
