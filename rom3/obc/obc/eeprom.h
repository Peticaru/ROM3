#ifndef _EEPROM_INTERFACE_H_
#define _EEPROM_INTERFACE_H_

#include <inttypes.h>

namespace eeprom
{
  uint8_t read(const uint8_t address);
  void write(const uint8_t address, const uint8_t value);
}

#endif
