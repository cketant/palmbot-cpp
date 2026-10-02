#pragma once
#include <cstdint>
/****************************************************************
Single source of truth for the Palmbot wiring map.

Pure data, no hardware imports -- this module is importable on any machine,
so fakes and tests can reason about the wiring without a Pi attached.
All numbers are BCM GPIO, matching gpiozero's default numbering.

########################################################

  # Pi 5 #        # JPT #        # DRV8833 #        # MAX98357A/I2S #      # LED RING #      # I2C #

# --------------------------------------------------------------------------------------------------------------------------------

######## WHEEL 1, MOTOR B

# ENCODERS
  # GPIO 17        yellow
  # GPIO 27        white
  # GPIO 5                           BIN1
  # GPIO 6                           BIN2


######## WHEEL 2, MOTOR A

# ENCODERS
  # GPIO 25        white
  # GPIO 24        yellow
  # GPIO 16                          AIN1
  # GPIO 12                          AIN2

######## GENERAL

  # GPIO 23                          STBY

# --------------------------------------------------------------------------------------------------------------------------------

  # GPIO 18                                               BCLK
  # GPIO 19                                               LRCLK
  # GPIO 20                                               DIN

# --------------------------------------------------------------------------------------------------------------------------------

  # GPIO 10                                                                SPI MOSI

# --------------------------------------------------------------------------------------------------------------------------------

  # GPIO 2                                                                                    SDA
  # GPIO 3                                                                                    SCL

# --------------------------------------------------------------------------------------------------------------------------------


***************************************************************/

namespace pins {
  using Gpio = std::uint8_t;

  inline constexpr Gpio SDA = 2;
  inline constexpr Gpio SCL = 3;
  
  inline constexpr Gpio SPI_MOSI = 10;

  inline constexpr Gpio I2S_BCLK = 18;
  inline constexpr Gpio I2S_LRCLK = 19;
  inline constexpr Gpio I2S_DIN = 20;

  struct Motor {
    Gpio in1;
    Gpio in2;
    Gpio enc1;
    Gpio enc2;
  };

  inline constexpr Motor leftMotor { 5, 6, 17, 27 };
  inline constexpr Motor rightMotor { 16, 12, 25, 24 };
  inline constexpr Gpio MOTOR_STANDBY = 23;
  
}