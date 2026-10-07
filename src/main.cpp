#include <iostream>
#include <thread>
#include <chrono>
#include "hal/linux_i2c_bus.h"
#include "drivers/mpu6050.h"

int main() {
  try {
    LinuxI2CBus bus("/dev/i2c-1");
    MPU6050 mpu6050(bus);
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      mpu6050.readData();
    };
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}
