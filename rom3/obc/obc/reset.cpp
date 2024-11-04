// 2022-10-13

#include "satellite_config.h"
#include "eeprom.h"
#include "satellite.h"
#include "reset.h"

uint8_t reset::get_count(void)
{
  uint8_t reset_count = eeprom::read(SATELLITE_EEPROM_RESET_COUNT);
  return reset_count;
}

void reset::inc_count(void)
{
  uint8_t reset_count = 0;
  reset_count = eeprom::read(SATELLITE_EEPROM_RESET_COUNT);

  if (reset_count == 255) // Avoid overflow and ignore 0
  {
    eeprom::write(SATELLITE_EEPROM_RESET_COUNT, 1);
  }
  else // Increse count
  {
    eeprom::write(SATELLITE_EEPROM_RESET_COUNT, ++reset_count);
  }
}

void reset::dec_count(void)
{
  uint8_t reset_count = eeprom::read(SATELLITE_EEPROM_RESET_COUNT);
  if (!(reset_count == 0))
  {
    eeprom::write(SATELLITE_EEPROM_RESET_COUNT, --reset_count);
  }
}

void reset::clear_count(void)
{
  eeprom::write(SATELLITE_EEPROM_RESET_COUNT, 0);
}

ResetStatus reset::get_status(void)
{
  ResetStatus output;
  uint8_t flag = eeprom::read(SATELLITE_EEPROM_RESET_FLAG);

  if (flag == 0)
  {
    output = ResetStatus::UNINTENTIONAL;
  }
  else if (flag == 1)
  {
    output = ResetStatus::INTENTIONAL;
  }
  return output;
}

void reset::set_status(ResetStatus rs)
{
  eeprom::write(SATELLITE_EEPROM_RESET_FLAG, uint8_t(rs));
}
