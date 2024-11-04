// 2022-11-03

#include "antenna.h"
#include "watchdog.h"
#include "satellite_config.h"
#include "eeprom.h"

/**
 * @brief Initialize deploy and sense pins.
 */
void antenna::init()
{
  pinMode(SATELLITE_PIN_ANTENNA_SENSE, INPUT);
  pinMode(SATELLITE_PIN_ANTENNA_DEPLOY, OUTPUT);
  digitalWrite(SATELLITE_PIN_ANTENNA_DEPLOY, LOW);
}

AntennaStatus antenna::get_status(void)
{
  // Check status from memory and sense switch
  uint8_t memory_status = eeprom::read(SATELLITE_EEPROM_ANT_DEP);
  uint8_t switch_status = digitalRead(SATELLITE_PIN_ANTENNA_SENSE);

  // Very first time after antenna deployment!
  if (switch_status & !memory_status)
  {
    // Update deployed antenna status on EEPROM
    antenna::set_status(DEPLOYED);
    memory_status = 1;
  }

  if (memory_status)
  {
    return DEPLOYED;
  }
  else
  {
    return NOT_DEPLOYED;
  }
}

void antenna::set_status(const enum AntennaStatus as)
{
  if (as == DEPLOYED)
  {
    eeprom::write(SATELLITE_EEPROM_ANT_DEP, 1);
  }
  else if (as == NOT_DEPLOYED)
  {
    eeprom::write(SATELLITE_EEPROM_ANT_DEP, 0);
  }
}

void antenna::deploy(void)
{
  // Deploy antenna for first time
  if (!eeprom::read(SATELLITE_EEPROM_BIRTH_FLAG))
  {
    if (antenna::get_status() == NOT_DEPLOYED)
    {
#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
      Serial.println("DEP-S");
#endif

      // Until the antenna is deployed
      uint8_t burn_counts = 0;
      while (antenna::get_status() == NOT_DEPLOYED & (burn_counts != SATELLITE_ANTENNA_BURNS))
      {
#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
        Serial.println("!DEP");
#endif
        watchdog::toggle();
        delay(SATELLITE_TIME_ANTENNA_WAIT);
        watchdog::toggle();
        digitalWrite(SATELLITE_PIN_ANTENNA_DEPLOY, HIGH); // FIRE!
        delay(SATELLITE_TIME_ANTENNA_BURN);               // Keep firing
        watchdog::toggle();
        digitalWrite(SATELLITE_PIN_ANTENNA_DEPLOY, LOW); // Stop

        burn_counts++;
      }

#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
      if (antenna::get_status())
      {
        Serial.println("DEP-E");
      }
      else
      {
        Serial.println("DEP-FAILED");
      }
#endif

      // Wait after burn
      for (uint8_t i = 0; i < SATELLITE_TIME_WAIT_AFTER_BURN; i++)
      {
        delay(30000);
        watchdog::toggle();
        delay(30000);
        watchdog::toggle();
      }
    }
  }
  eeprom::write(SATELLITE_EEPROM_BIRTH_FLAG, 1);
}
