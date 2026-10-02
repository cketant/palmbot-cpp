#pragma once

#include <cstdint>

/**
 * HAL 
 */
class I2CBus {

  public:

    virtual bool readReg(std::uint8_t reg, std::uint8_t &value) = 0;
    
    virtual bool writeReg(std::uint8_t reg, std::uint8_t value) = 0;

    virtual void whoAmI(std::uint8_t reg) = 0;

    virtual ~I2CBus() = default;
    
    I2CBus& operator=(const I2CBus&) = delete;

    I2CBus(const I2CBus&) = delete;

  protected:
  
    I2CBus() = default;
    
};