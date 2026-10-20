#include <iostream>
#include <chrono>
#include <thread>
#include "hal/linux_i2c_bus.h"
#include "drivers/mpu6050.h"
#include "drivers/vl53l0x.h"

int main() {
  try {
    LinuxI2CBus bus("/dev/i2c-1");
    MPU6050 mpu6050(bus);
    VL53L0X vl53l0x(bus);
    
    rigtorp::SPSCQueue<Measurement> queue(1024);

    std::chrono::steady_clock::time_point::duration period = std::chrono::milliseconds(10);
    std::jthread imuThread{[&](){
      auto timerStart = std::chrono::steady_clock::now();
      auto next = std::chrono::steady_clock::now();
      while((std::chrono::steady_clock::now() - timerStart) <= std::chrono::seconds(30)) {
        next += period;
        auto m = mpu6050.measurement();
        if (m) {
          if (!queue.try_push(m.value())) std::cout << "Failed Push";
        }
        std::this_thread::sleep_until(next);
      }
    }};

    std::jthread loggerThread([&](std::stop_token st) {
      std::optional<std::chrono::steady_clock::time_point> t0;
      while (true) {
        bool isStopping = st.stop_requested();
        if (Measurement* m = queue.front()) {
          // log m
          if (const auto* imu = std::get_if<IMUMeasurement>(&m->data)) {
            if (!t0) t0 = m->timestamp;
            auto elapsedTime = std::chrono::duration<double>(m->timestamp - t0.value()).count();
            std::cout << "Time: " << elapsedTime << "s | " << "Temperature: " << imu->tempC << std::endl;
          }
          queue.pop();
        } else if (isStopping && !queue.front()) {
          break;
        } else {
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
      }
    });

    imuThread.join();
    loggerThread.request_stop();
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}
