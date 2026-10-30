#include <iostream>
#include <chrono>
#include <thread>
#include <stop_token>

#include "config/pins.h"
#include "hal/linux_i2c_bus.h"
#include "drivers/mpu6050.h"
#include "drivers/vl53l0x.h"
#include "drivers/drv8833.h"

int main() {

  try {
    LinuxI2CBus bus("/dev/i2c-1");
    MPU6050 mpu6050(bus);
    VL53L0X vl53l0x(bus);
    DRV8833 drv8833(pins::leftMotor, pins::rightMotor, pins::MOTOR_STANDBY);
    
    rigtorp::SPSCQueue<Measurement> queue(1024);
    std::atomic<std::int64_t> leftTicks {0};

    std::chrono::steady_clock::time_point::duration period = std::chrono::milliseconds(10);
    const auto loggingDuration = std::chrono::seconds(30);
    const auto timerStart = std::chrono::steady_clock::now();

    std::jthread imuThread{[&](std::stop_token st){
      auto next = std::chrono::steady_clock::now();
      std::int64_t lastTicksCount {0};
      while((std::chrono::steady_clock::now() - timerStart) <= loggingDuration) {
        next += period;
        auto m = mpu6050.measurement();
        if (m) {
          std::int64_t currTicksCount = leftTicks.load(std::memory_order_relaxed);
          m->ticks = currTicksCount - lastTicksCount;
          lastTicksCount = currTicksCount;
          if (!queue.try_push(m.value())) std::cout << "Failed Push";
        }
        std::this_thread::sleep_until(next);
      }
    }};

    std::jthread encoderThread{[&](std::stop_token st){
      while (!st.stop_requested()) {
        leftTicks.fetch_add(
          drv8833.readLeftMotorEncoderTicks(std::chrono::milliseconds(50)),
          std::memory_order_relaxed
        );
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
            std::cout << "Time: " << elapsedTime << "s | " << "Temperature: " << imu->tempC << "°C | " << "Ticks: " << m->ticks  << std::endl;
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
    encoderThread.request_stop();
    loggerThread.request_stop();
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}
