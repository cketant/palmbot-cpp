#include <iostream>
#include <chrono>
#include <thread>
#include <stop_token>

#include "config/pins.h"
#include "hal/linux_i2c_bus.h"
#include "drivers/mpu6050.h"
#include "drivers/vl53l0x.h"
#include "drivers/drv8833.h"

void logging(MPU6050& mpu6050, VL53L0X& vl53l0x, DRV8833& drv8833) {
    rigtorp::SPSCQueue<LogMeasurement> queue(1024);
    std::atomic<std::int64_t> leftTicks {0};
    std::atomic<std::int64_t> rightTicks {0};
    std::atomic<float> currTofDist {-1};

    std::chrono::steady_clock::time_point::duration period = std::chrono::milliseconds(10);
    const auto loggingDuration = std::chrono::seconds(30);
    const auto timerStart = std::chrono::steady_clock::now();

    /**
     * Fill the Log Queue based on when we fetch the latest 
     * IMU readings. 
     */
    std::jthread imuThread{[&](std::stop_token st){
      auto next = std::chrono::steady_clock::now();
      std::int64_t lastLeftTicksCount {0};
      std::int64_t lastRightTicksCount {0};
      LogMeasurement log {};
      while((std::chrono::steady_clock::now() - timerStart) <= loggingDuration) {
        next += period;

        // IMU 
        auto m = mpu6050.measurement();
        if (m) {
          log.tempC = std::get<IMUMeasurement>(m->data).tempC;
          log.timestamp = m->timestamp;
        } else {
          log.tempC = -1;
        }

        // Encoder
        std::int64_t currLeftTicksCount = leftTicks.load(std::memory_order_relaxed);
        std::int64_t currRightTicksCount = rightTicks.load(std::memory_order_relaxed);
        log.leftMotorticks = (currLeftTicksCount - lastLeftTicksCount);
        log.rightMotorticks = (currRightTicksCount - lastRightTicksCount);
        lastLeftTicksCount = currLeftTicksCount;
        lastRightTicksCount = currRightTicksCount;

        // TOF 
        log.distance = currTofDist;

        if (!queue.try_push(log)) std::cout << "Failed Push";
        std::this_thread::sleep_until(next);
      }
    }};

    /**
     * Accumulates the total number of encoder ticks
     */
    std::jthread encoderThread{[&](std::stop_token st){
      auto timeoutMS = std::chrono::milliseconds(50);
      while (!st.stop_requested()) {
        leftTicks.fetch_add(drv8833.readLeftMotorEncoderTicks(timeoutMS), std::memory_order_relaxed);
        rightTicks.fetch_add(drv8833.readRightMotorEncoderTicks(timeoutMS), std::memory_order_relaxed);
      }
    }};

    /**
     * Updates the latest value buffer representing the ToF distance
     */
    std::jthread tofThread{[&](std::stop_token st) {
      while (!st.stop_requested()) {
        auto m = vl53l0x.measurement();
        if (m) {
          float d = std::get<TOFMeasurement>(m->data).distance;
          currTofDist.store(d, std::memory_order_relaxed);
        }
      }
    }};

    std::jthread loggerThread([&](std::stop_token st) {
      std::optional<std::chrono::steady_clock::time_point> t0;
      while (true) {
        bool isStopping = st.stop_requested();
        if (LogMeasurement* log = queue.front()) {
          // log m
          if (!t0) t0 = log->timestamp;
          auto elapsedTime = std::chrono::duration<double>(log->timestamp - t0.value()).count();
          std::cout << "Time: " << elapsedTime << "s | " << "Temperature: " << log->tempC << "°C | " << "Left Ticks: " << log->leftMotorticks << " | ToF Distance: " << log->distance  << std::endl;
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
    tofThread.request_stop();
    loggerThread.request_stop();
}

void driving(DRV8833& drv8833) {
  auto duration = std::chrono::seconds(3);

  auto startLeft = std::chrono::steady_clock::now();
  while(std::chrono::steady_clock::now() - startLeft <= duration) {
    drv8833.turnLeft();
  }

  auto startRight = std::chrono::steady_clock::now();
  while(std::chrono::steady_clock::now() - startRight <= duration) {
    drv8833.turnRight();
  }
}

int main() {

  try {
    LinuxI2CBus bus("/dev/i2c-1");
    MPU6050 mpu6050(bus);
    VL53L0X vl53l0x(bus);
    DRV8833 drv8833(pins::leftMotor, pins::rightMotor, pins::MOTOR_STANDBY);

    // logging(mpu6050, vl53l0x, drv8833);
    driving(drv8833);
    
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}
