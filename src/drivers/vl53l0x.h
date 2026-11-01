#pragma once

#include <iostream>
#include <cstdio>
#include <iomanip>
#include <array>
#include <chrono>
#include <optional>

#include "lib/SPSCQueue.h"
#include "hal/i2c_device.h"


class VL53L0X: public I2CDevice {

  public:

    VL53L0X(I2CBus& bus, 
      std::uint8_t deviceAddr = 0x29, 
      std::string deviceName = "VL530X");

    ~VL53L0X();

    /**
     * Print the WHO AM I value
     */
    std::uint8_t whoAmI() override;

    /**
     * The measurement for the sensor
     */
    std::optional<Measurement> measurement() override;

    /**
     * Configure the sensor after wake
     */
    bool configure() override;

    /**
     * Wake the sesnsor
     */
    bool wake() override;

    /**
     * Reset the sensor
     */
    bool reset() override;

    static constexpr std::uint8_t SYSRANGE_START = 0x00;

    static constexpr std::uint8_t SYSTEM_INTERMEASUREMENT_PERIOD = 0x04;

    static constexpr std::uint8_t OSC_CALIBRATE_VAL = 0xF8;

    static constexpr std::uint8_t RESULT_INTERRUPT_STATUS = 0x13;

    static constexpr std::uint8_t RESULT_RANGE_STATUS = 0x14;

    static constexpr std::uint8_t SYSTEM_INTERRUPT_CLEAR = 0x0B;

    static constexpr std::uint8_t SYSTEM_SEQUENCE_CONFIG = 0x01;

    static constexpr std::uint8_t SYSTEM_INTERRUPT_CONFIG_GPIO = 0x0A;

  private:

    /**
     * Stop variable read during configure(), needed to start ranging
     */
    std::uint8_t stopVariable_ = 0;

    /**
     * Maximum time to wait on a register poll
     */
    static constexpr std::chrono::milliseconds kTimeout{500};

    bool writeReg8(std::uint8_t reg, std::uint8_t val);

    bool writeReg16(std::uint8_t reg, std::uint16_t val);

    bool writeReg32(std::uint8_t reg, std::uint32_t val);

    bool readReg8(std::uint8_t reg, std::uint8_t& val);

    /**
     * Big-endian 16-bit read
     */
    bool readReg16(std::uint8_t reg, std::uint16_t& val);

    /**
     * Poll until (reg & mask) is nonzero, or zero when waitClear, within kTimeout
     */
    bool waitReg(std::uint8_t reg, std::uint8_t mask, bool waitClear);

    /**
     * Write the stop variable back to the sensor, required before starting a measurement
     */
    bool restoreStopVariable();

    /**
     * Start continuous ranging. With a zero period the sensor measures back-to-back
     * (as often as the timing budget allows); otherwise it measures once per period.
     * Based on VL53L0X_StartMeasurement()
     *
     * @param period Inter-measurement period, 0 for back-to-back mode
     */
    bool startContinuous(std::chrono::milliseconds period = std::chrono::milliseconds{0});

    /**
     * Stop continuous ranging
     * Based on VL53L0X_StopMeasurement()
     */
    bool stopContinuous();

    /**
     * Returns a range reading in millimeters when continuous mode is active.
     * Also used by blockingReadRangeSingleMillimeters() after it starts a single-shot.
     * Based on VL53L0X_PerformSingleRangingMeasurement()
     *
     * @return The range, or nullopt on a bus error or timeout
     */
    std::optional<std::uint16_t> blockingReadRangeContinuousMillimeters();

    /**
     * Performs a single-shot range measurement
     *
     * @return The range in millimeters, or nullopt on a bus error or timeout
     */
    std::optional<std::uint16_t> blockingReadRangeSingleMillimeters();

};