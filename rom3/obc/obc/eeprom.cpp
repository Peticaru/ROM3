#include <EEPROM.h>
#include "satellite_config.h"
#include "eeprom.h"


uint8_t eeprom::read(const uint8_t address)
{
  uint8_t output = 0;

  // Read three redundant status registers
  const uint8_t address_1 = address + SATELLITE_EEPROM_OFFSET_1;
  const uint8_t address_2 = address + SATELLITE_EEPROM_OFFSET_2;
  const uint8_t address_3 = address + SATELLITE_EEPROM_OFFSET_3;
  uint8_t value_1 = EEPROM.read(address_1);
  uint8_t value_2 = EEPROM.read(address_2);
  uint8_t value_3 = EEPROM.read(address_3);

  // Check if all values are same
  if (value_1 == value_2 && value_2 == value_3)
  {
    output = value_1;
  }
  // Handle latch-up events
  else if (value_1 == value_2)
  {
    EEPROM.write(address_3, value_1);
    output = value_1;
  }
  else if (value_2 == value_3)
  {
    EEPROM.write(address_1, value_2);
    output = value_2;
  }
  else if (value_3 == value_1)
  {
    EEPROM.write(address_2, value_3);
    output = value_3;
  }

  return output;
}

void eeprom::write(const uint8_t address, const uint8_t value)
{
  // Write to all addresses including the redundant ones
  EEPROM.write(address + SATELLITE_EEPROM_OFFSET_1, value);
  EEPROM.write(address + SATELLITE_EEPROM_OFFSET_2, value);
  EEPROM.write(address + SATELLITE_EEPROM_OFFSET_3, value);
}
