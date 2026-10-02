#include <iostream>
#include "drivers/linux_i2c_bus.h"

int main() {
  LinuxI2CBus bus("/dev/i2c-1", 0x68);
  bus.whoAmI(0x75);
  return 0;
}
