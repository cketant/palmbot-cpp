#include <iostream>
#include <chrono>
#include <thread>
#include "hal/linux_i2c_bus.h"
#include "drivers/mpu6050.h"

int main() {
  try {
    LinuxI2CBus bus("/dev/i2c-1");
    MPU6050 mpu6050(bus);
    rigtorp::SPSCQueue<Measurement> queue(1024);

    std::chrono::steady_clock::time_point::duration period = std::chrono::milliseconds(10);
    std::chrono::steady_clock::time_point next = std::chrono::steady_clock::now();

    std::jthread imuThread{[&](std::stop_token st){
      while(!st.stop_requested()) {
        next += period;
        auto m = mpu6050.measurement();
        if (m) {
          queue.try_push(m.value());
        }
        std::this_thread::sleep_until(next);
      }
    }};


    // while (true) {
    //   std::optional<Measurement> m = mpu6050.measurement();
    // };
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}
