#include "vl53l0x.h"

#include <algorithm>
#include <chrono>
#include <stdexcept>

VL53L0X::VL53L0X(I2CBus& bus, 
  const std::uint8_t deviceAddr, 
  const std::string deviceName) : I2CDevice(bus, deviceAddr, deviceName) {
    if (!configure()) throw std::runtime_error(deviceName_ + ": configure failed");
    if (!wake()) throw std::runtime_error(deviceName_ + ": wake failed");
};

VL53L0X::~VL53L0X() {
  stopContinuous();
};

std::uint8_t VL53L0X::whoAmI() {
  std::array<std::uint8_t, 1> whoAmIBuff{};
  if (!bus_.readReg(addr_, 0xC0, whoAmIBuff)) return -1;
  std::cout << deviceName_ << " - " << "WHO_AM_I: " << std::hex << std::showbase << (int)whoAmIBuff[0] << std::endl;
  return whoAmIBuff[0];
};

bool VL53L0X::configure() {
  // Register map (only those not already declared in the header)
  constexpr std::uint8_t IDENTIFICATION_MODEL_ID = 0xC0;
  constexpr std::uint8_t VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV = 0x89;
  constexpr std::uint8_t MSRC_CONFIG_CONTROL = 0x60;
  constexpr std::uint8_t MSRC_CONFIG_TIMEOUT_MACROP = 0x46;
  constexpr std::uint8_t FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT = 0x44;
  constexpr std::uint8_t GLOBAL_CONFIG_SPAD_ENABLES_REF_0 = 0xB0;
  constexpr std::uint8_t GLOBAL_CONFIG_REF_EN_START_SELECT = 0xB6;
  constexpr std::uint8_t DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD = 0x4E;
  constexpr std::uint8_t DYNAMIC_SPAD_REF_EN_START_OFFSET = 0x4F;
  constexpr std::uint8_t GPIO_HV_MUX_ACTIVE_HIGH = 0x84;
  constexpr std::uint8_t PRE_RANGE_CONFIG_VCSEL_PERIOD = 0x50;
  constexpr std::uint8_t PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x51;
  constexpr std::uint8_t FINAL_RANGE_CONFIG_VCSEL_PERIOD = 0x70;
  constexpr std::uint8_t FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x71;

  // Read-modify-write of an 8-bit register
  auto modify = [&](std::uint8_t reg, auto fn) {
    std::uint8_t v = 0;
    return readReg8(reg, v) && writeReg8(reg, fn(v));
  };

  // Timing conversions (VL53L0X_calc_macro_period_ps / calc_timeout_us / calc_timeout_mclks)
  auto macroPeriodNs = [](std::uint8_t vcselPclks) -> std::uint32_t {
    return ((std::uint32_t{2304} * vcselPclks * 1655) + 500) / 1000;
  };
  auto mclksToUs = [&](std::uint16_t mclks, std::uint8_t vcselPclks) -> std::uint32_t {
    return ((mclks * macroPeriodNs(vcselPclks)) + 500) / 1000;
  };
  auto usToMclks = [&](std::uint32_t us, std::uint8_t vcselPclks) -> std::uint32_t {
    const std::uint32_t mp = macroPeriodNs(vcselPclks);
    return ((us * 1000) + (mp / 2)) / mp;
  };
  auto decodeTimeout = [](std::uint16_t reg) -> std::uint16_t {
    return static_cast<std::uint16_t>((reg & 0x00FF) << ((reg & 0xFF00) >> 8)) + 1;
  };
  auto encodeTimeout = [](std::uint32_t mclks) -> std::uint16_t {
    if (mclks == 0) return 0;
    std::uint32_t ls = mclks - 1;
    std::uint16_t ms = 0;
    while ((ls & 0xFFFFFF00) > 0) { ls >>= 1; ms++; }
    return static_cast<std::uint16_t>((ms << 8) | (ls & 0xFF));
  };

  struct Steps {
    bool tcc, dss, msrc, preRange, finalRange;
    std::uint8_t preVcsel, finalVcsel;
    std::uint16_t preMclks;
    std::uint32_t msrcUs, preUs, finalUs;
    std::uint32_t finalMclks;
  };
  // VL53L0X_GetSequenceStepEnables + get_sequence_step_timeout for every step
  auto readSteps = [&](Steps& s) {
    std::uint8_t seq = 0, v = 0;
    std::uint16_t r16 = 0;
    if (!readReg8(SYSTEM_SEQUENCE_CONFIG, seq)) return false;
    s.tcc = (seq >> 4) & 1;
    s.dss = (seq >> 3) & 1;
    s.msrc = (seq >> 2) & 1;
    s.preRange = (seq >> 6) & 1;
    s.finalRange = (seq >> 7) & 1;

    if (!readReg8(PRE_RANGE_CONFIG_VCSEL_PERIOD, v)) return false;
    s.preVcsel = (v + 1) << 1;
    if (!readReg8(MSRC_CONFIG_TIMEOUT_MACROP, v)) return false;
    s.msrcUs = mclksToUs(v + 1, s.preVcsel);
    if (!readReg16(PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI, r16)) return false;
    s.preMclks = decodeTimeout(r16);
    s.preUs = mclksToUs(s.preMclks, s.preVcsel);

    if (!readReg8(FINAL_RANGE_CONFIG_VCSEL_PERIOD, v)) return false;
    s.finalVcsel = (v + 1) << 1;
    if (!readReg16(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI, r16)) return false;
    s.finalMclks = decodeTimeout(r16);
    if (s.preRange) s.finalMclks -= s.preMclks;
    s.finalUs = mclksToUs(static_cast<std::uint16_t>(s.finalMclks), s.finalVcsel);
    return true;
  };

  constexpr std::uint16_t kStartOverhead = 1910, kEndOverhead = 960, kMsrcOverhead = 660,
                          kTccOverhead = 590, kDssOverhead = 690, kPreRangeOverhead = 660,
                          kFinalRangeOverhead = 550;
  // Overhead + all non-final steps, shared by get/set measurement timing budget
  auto usedBudget = [&](const Steps& s) {
    std::uint32_t used = kStartOverhead + kEndOverhead;
    if (s.tcc) used += s.msrcUs + kTccOverhead;
    if (s.dss) used += 2 * (s.msrcUs + kDssOverhead);
    else if (s.msrc) used += s.msrcUs + kMsrcOverhead;
    if (s.preRange) used += s.preUs + kPreRangeOverhead;
    return used;
  };

  // VL53L0X_perform_single_ref_calibration()
  auto singleRefCalibration = [&](std::uint8_t vhvInitByte) {
    return writeReg8(SYSRANGE_START, 0x01 | vhvInitByte)
        && waitReg(RESULT_INTERRUPT_STATUS, 0x07, false)
        && writeReg8(SYSTEM_INTERRUPT_CLEAR, 0x01)
        && writeReg8(SYSRANGE_START, 0x00);
  };

  // Check model ID register (value specified in datasheet)
  // std::uint8_t modelId = 0;
  // if (!readReg8(IDENTIFICATION_MODEL_ID, modelId) || modelId != 0xEE) return false;

  // VL53L0X_DataInit() begin

  // Sensor uses 1V8 mode for I/O by default; switch to 2V8 mode
  if (!modify(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, [](std::uint8_t v) -> std::uint8_t { return v | 0x01; })) return false;

  // "Set I2C standard mode" and read the stop variable
  if (!writeReg8(0x88, 0x00)) return false;
  if (!writeReg8(0x80, 0x01) || !writeReg8(0xFF, 0x01) || !writeReg8(0x00, 0x00)) return false;
  if (!readReg8(0x91, stopVariable_)) return false;
  if (!writeReg8(0x00, 0x01) || !writeReg8(0xFF, 0x00) || !writeReg8(0x80, 0x00)) return false;

  // Disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
  if (!modify(MSRC_CONFIG_CONTROL, [](std::uint8_t v) -> std::uint8_t { return v | 0x12; })) return false;

  // Final range signal rate limit 0.25 MCPS, Q9.7 fixed point
  if (!writeReg16(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, static_cast<std::uint16_t>(0.25f * (1 << 7)))) return false;

  if (!writeReg8(SYSTEM_SEQUENCE_CONFIG, 0xFF)) return false;

  // VL53L0X_DataInit() end

  // VL53L0X_StaticInit() begin

  // Reference SPAD count and type (VL53L0X_get_info_from_device())
  std::uint8_t spadInfo = 0;
  if (!writeReg8(0x80, 0x01) || !writeReg8(0xFF, 0x01) || !writeReg8(0x00, 0x00)) return false;
  if (!writeReg8(0xFF, 0x06)) return false;
  if (!modify(0x83, [](std::uint8_t v) -> std::uint8_t { return v | 0x04; })) return false;
  if (!writeReg8(0xFF, 0x07) || !writeReg8(0x81, 0x01) || !writeReg8(0x80, 0x01)) return false;
  if (!writeReg8(0x94, 0x6B) || !writeReg8(0x83, 0x00)) return false;
  if (!waitReg(0x83, 0xFF, false)) return false;
  if (!writeReg8(0x83, 0x01) || !readReg8(0x92, spadInfo)) return false;
  if (!writeReg8(0x81, 0x00) || !writeReg8(0xFF, 0x06)) return false;
  if (!modify(0x83, [](std::uint8_t v) -> std::uint8_t { return v & ~0x04; })) return false;
  if (!writeReg8(0xFF, 0x01) || !writeReg8(0x00, 0x01) || !writeReg8(0xFF, 0x00) || !writeReg8(0x80, 0x00)) return false;

  const std::uint8_t spadCount = spadInfo & 0x7F;
  const bool spadTypeIsAperture = (spadInfo >> 7) & 0x01;

  // The SPAD map is readable from GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _5
  std::array<std::uint8_t, 6> refSpadMap{};
  if (!bus_.readReg(addr_, GLOBAL_CONFIG_SPAD_ENABLES_REF_0, refSpadMap)) return false;

  // -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)

  if (!writeReg8(0xFF, 0x01) || !writeReg8(DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00)
      || !writeReg8(DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C) || !writeReg8(0xFF, 0x00)
      || !writeReg8(GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4)) return false;

  const std::uint8_t firstSpadToEnable = spadTypeIsAperture ? 12 : 0; // 12 is the first aperture spad
  std::uint8_t spadsEnabled = 0;
  for (std::uint8_t i = 0; i < 48; i++) {
    if (i < firstSpadToEnable || spadsEnabled == spadCount) {
      refSpadMap[i / 8] &= ~(1 << (i % 8));
    } else if ((refSpadMap[i / 8] >> (i % 8)) & 0x1) {
      spadsEnabled++;
    }
  }

  std::array<std::uint8_t, 7> spadWrite{GLOBAL_CONFIG_SPAD_ENABLES_REF_0};
  std::copy(refSpadMap.begin(), refSpadMap.end(), spadWrite.begin() + 1);
  if (!bus_.writeReg(addr_, spadWrite)) return false;

  // -- VL53L0X_set_reference_spads() end

  // -- VL53L0X_load_tuning_settings() begin (DefaultTuningSettings from vl53l0x_tuning.h)
  static constexpr auto kTuning = std::to_array<std::array<std::uint8_t, 2>>({
    {0xFF, 0x01}, {0x00, 0x00},
    {0xFF, 0x00}, {0x09, 0x00}, {0x10, 0x00}, {0x11, 0x00},
    {0x24, 0x01}, {0x25, 0xFF}, {0x75, 0x00},
    {0xFF, 0x01}, {0x4E, 0x2C}, {0x48, 0x00}, {0x30, 0x20},
    {0xFF, 0x00}, {0x30, 0x09}, {0x54, 0x00}, {0x31, 0x04}, {0x32, 0x03}, {0x40, 0x83}, {0x46, 0x25}, {0x60, 0x00}, {0x27, 0x00}, {0x50, 0x06}, {0x51, 0x00}, {0x52, 0x96}, {0x56, 0x08}, {0x57, 0x30}, {0x61, 0x00}, {0x62, 0x00}, {0x64, 0x00}, {0x65, 0x00}, {0x66, 0xA0},
    {0xFF, 0x01}, {0x22, 0x32}, {0x47, 0x14}, {0x49, 0xFF}, {0x4A, 0x00},
    {0xFF, 0x00}, {0x7A, 0x0A}, {0x7B, 0x00}, {0x78, 0x21},
    {0xFF, 0x01}, {0x23, 0x34}, {0x42, 0x00}, {0x44, 0xFF}, {0x45, 0x26}, {0x46, 0x05}, {0x40, 0x40}, {0x0E, 0x06}, {0x20, 0x1A}, {0x43, 0x40},
    {0xFF, 0x00}, {0x34, 0x03}, {0x35, 0x44},
    {0xFF, 0x01}, {0x31, 0x04}, {0x4B, 0x09}, {0x4C, 0x05}, {0x4D, 0x04},
    {0xFF, 0x00}, {0x44, 0x00}, {0x45, 0x20}, {0x47, 0x08}, {0x48, 0x28}, {0x67, 0x00}, {0x70, 0x04}, {0x71, 0x01}, {0x72, 0xFE}, {0x76, 0x00}, {0x77, 0x00},
    {0xFF, 0x01}, {0x0D, 0x01},
    {0xFF, 0x00}, {0x80, 0x01}, {0x01, 0xF8},
    {0xFF, 0x01}, {0x8E, 0x01}, {0x00, 0x01}, {0xFF, 0x00}, {0x80, 0x00},
    {SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04}, 
  });

  for (auto entry : kTuning) {
    if (!bus_.writeReg(addr_, entry)) return false;
  }

  // -- VL53L0X_load_tuning_settings() end

  // "Set interrupt config to new sample ready" (SYSTEM_INTERRUPT_CONFIG_GPIO written by the table)
  if (!modify(GPIO_HV_MUX_ACTIVE_HIGH, [](std::uint8_t v) -> std::uint8_t { return v & ~0x10; })) return false; // active low
  if (!writeReg8(SYSTEM_INTERRUPT_CLEAR, 0x01)) return false;

  // Current measurement timing budget (all steps enabled, sequence config 0xFF)
  Steps steps{};
  if (!readSteps(steps)) return false;
  std::uint32_t budgetUs = usedBudget(steps);
  if (steps.finalRange) budgetUs += steps.finalUs + kFinalRangeOverhead;

  // "Disable MSRC and TCC by default"
  if (!writeReg8(SYSTEM_SEQUENCE_CONFIG, 0xE8)) return false;

  // "Recalculate timing budget" (VL53L0X_set_measurement_timing_budget_micro_seconds())
  if (!readSteps(steps)) return false;
  if (steps.finalRange) {
    std::uint32_t used = usedBudget(steps) + kFinalRangeOverhead;
    if (used > budgetUs) return false; // "Requested timeout too big."
    std::uint32_t finalMclks = usToMclks(budgetUs - used, steps.finalVcsel);
    if (steps.preRange) finalMclks += steps.preMclks;
    if (!writeReg16(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI, encodeTimeout(finalMclks))) return false;
  }

  // VL53L0X_StaticInit() end

  // VL53L0X_PerformRefCalibration() begin

  if (!writeReg8(SYSTEM_SEQUENCE_CONFIG, 0x01) || !singleRefCalibration(0x40)) return false; // VHV
  if (!writeReg8(SYSTEM_SEQUENCE_CONFIG, 0x02) || !singleRefCalibration(0x00)) return false; // phase

  // "restore the previous Sequence Config"
  if (!writeReg8(SYSTEM_SEQUENCE_CONFIG, 0xE8)) return false;

  // VL53L0X_PerformRefCalibration() end

  return true;
};

bool VL53L0X::writeReg8(std::uint8_t reg, std::uint8_t val) {
  std::array<std::uint8_t, 2> buf{reg, val};
  return bus_.writeReg(addr_, buf);
};

bool VL53L0X::writeReg16(std::uint8_t reg, std::uint16_t val) {
  std::array<std::uint8_t, 3> buf{reg, static_cast<std::uint8_t>(val >> 8), static_cast<std::uint8_t>(val)};
  return bus_.writeReg(addr_, buf);
};

bool VL53L0X::writeReg32(std::uint8_t reg, std::uint32_t val) {
  std::array<std::uint8_t, 5> buf{reg,
    static_cast<std::uint8_t>(val >> 24), static_cast<std::uint8_t>(val >> 16),
    static_cast<std::uint8_t>(val >> 8), static_cast<std::uint8_t>(val)};
  return bus_.writeReg(addr_, buf);
};

bool VL53L0X::readReg8(std::uint8_t reg, std::uint8_t& val) {
  std::array<std::uint8_t, 1> buf{};
  try {
    if (!bus_.readReg(addr_, reg, buf)) return false;
    val = buf[0];
  } catch (const std::exception&) {
    return false;
  }
  return true;
};

bool VL53L0X::readReg16(std::uint8_t reg, std::uint16_t& val) {
  std::array<std::uint8_t, 2> buf{};
  if (!bus_.readReg(addr_, reg, buf)) return false;
  val = static_cast<std::uint16_t>((buf[0] << 8) | buf[1]);
  return true;
};

bool VL53L0X::waitReg(std::uint8_t reg, std::uint8_t mask, bool waitClear) {
  const auto start = std::chrono::steady_clock::now();
  std::uint8_t v = 0;
  while (true) {
    if (!readReg8(reg, v)) return false;
    if (((v & mask) == 0) == waitClear) return true;
    if (std::chrono::steady_clock::now() - start > kTimeout) return false;
  }
};

bool VL53L0X::restoreStopVariable() {
  return writeReg8(0x80, 0x01)
      && writeReg8(0xFF, 0x01)
      && writeReg8(0x00, 0x00)
      && writeReg8(0x91, stopVariable_)
      && writeReg8(0x00, 0x01)
      && writeReg8(0xFF, 0x00)
      && writeReg8(0x80, 0x00);
};

bool VL53L0X::startContinuous(std::chrono::milliseconds period) {
  if (!restoreStopVariable()) return false;

  if (period.count() == 0) {
    // Continuous back-to-back mode
    return writeReg8(SYSRANGE_START, 0x02);
  }

  // Continuous timed mode
  // VL53L0X_SetInterMeasurementPeriodMilliSeconds() begin
  std::uint16_t oscCalibrateVal = 0;
  if (!readReg16(OSC_CALIBRATE_VAL, oscCalibrateVal)) return false;

  auto periodMs = static_cast<std::uint32_t>(period.count());
  if (oscCalibrateVal != 0) periodMs *= oscCalibrateVal;

  if (!writeReg32(SYSTEM_INTERMEASUREMENT_PERIOD, periodMs)) return false;
  // VL53L0X_SetInterMeasurementPeriodMilliSeconds() end

  return writeReg8(SYSRANGE_START, 0x04);
};

bool VL53L0X::stopContinuous() {
  return writeReg8(SYSRANGE_START, 0x01)
      && writeReg8(0xFF, 0x01)
      && writeReg8(0x00, 0x00)
      && writeReg8(0x91, 0x00)
      && writeReg8(0x00, 0x01)
      && writeReg8(0xFF, 0x00);
};

std::optional<std::uint16_t> VL53L0X::blockingReadRangeContinuousMillimeters() {
  // Wait for a new sample (any of the interrupt status bits)
  if (!waitReg(RESULT_INTERRUPT_STATUS, 0x07, false)) return std::nullopt;

  // Assumes Linearity Corrective Gain is 1000 (default) and fractional ranging is not enabled
  std::uint16_t range = 0;
  if (!readReg16(RESULT_RANGE_STATUS + 10, range)) return std::nullopt;

  if (!writeReg8(SYSTEM_INTERRUPT_CLEAR, 0x01)) return std::nullopt;

  return range;
};

std::optional<std::uint16_t> VL53L0X::blockingReadRangeSingleMillimeters() {
  if (!restoreStopVariable()) return std::nullopt;

  if (!writeReg8(SYSRANGE_START, 0x01)) return std::nullopt;

  // "Wait until start bit has been cleared"
  if (!waitReg(SYSRANGE_START, 0x01, true)) return std::nullopt;

  return blockingReadRangeContinuousMillimeters();
};

bool VL53L0X::wake() {
  return startContinuous(std::chrono::milliseconds(30));
};

bool VL53L0X::reset() {
  return true;
};

std::optional<Measurement> VL53L0X::measurement() {
  auto mm = blockingReadRangeContinuousMillimeters();
  if (!mm) return std::nullopt;

  // 8190 and above are the sensor's "out of range" values, not real distances
  if (*mm >= 8190) return std::nullopt;

  // Distance in meters, matching the SI units used by the IMU
  TOFMeasurement tof{static_cast<float>(*mm) / 1000.0f};
  return Measurement{std::chrono::steady_clock::now(), tof};
};