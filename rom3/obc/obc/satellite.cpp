#include <EEPROM.h>
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include "reset.h"
#include "sensor.h"
#include "antenna.h"
#include "payload.h"
#include "watchdog.h"
#include "satellite.h"
#include "communication.h"
#include "eeprom.h"
#include "RF4463.h"

extern RF4463 rf4463;

void preinitialize_eeprom_params(void)
{
#if SATELLITE_RESET_ANTENNA_STATUS == 0
  // Ready for flight message
  if (!antenna::get_status())
  {
    Serial.begin(115200);
    Serial.println("Ready to fly!");

// Disable serial if no debug mode
#if (SATELLITE_ENABLE_SERIAL_DEBUG == 0)
    Serial.end();
#endif
  }
#endif

/**
 * @brief Settings for various EEPROM flags just before flashing the very last
 *  satellite firmware before launch.
 * @note Make sure that the antenna deploy switch stays bound and is not
 *  activated just before and never after flashing the code with macro set to 1.
 */
#if SATELLITE_RESET_ANTENNA_STATUS == 1
  eeprom::write(SATELLITE_EEPROM_BIRTH_FLAG, 0);          // Satellite is not born yet
  eeprom::write(SATELLITE_EEPROM_TX_IMAGE_FLAG, 1);       // Set image transmission flag
  eeprom::write(SATELLITE_EEPROM_RESET_DELAY_COUNTER, 0); // Reset timing delays
  eeprom::write(SATELLITE_EEPROM_TX_IMAGE_SW, 1);         // Enable image transmission
  eeprom::write(SATELLITE_EEPROM_TX_GFSK_SW, 1);          // Enable GFSK data Tx
  eeprom::write(SATELLITE_EEPROM_TX_CW_SW, 1);            // Enable CW data Tx
  eeprom::write(SATELLITE_EEPROM_IMAGE_TOGGLE, 1);        // Capture from camera
  antenna::set_status(NOT_DEPLOYED);                      // Reset deployment status
  reset::clear_count();                                   // Reset counts = 0;
  
  // Set unintentional reset mode
  reset::set_status(ResetStatus::UNINTENTIONAL);

  Serial.begin(115200);
  if (antenna::get_status() == DEPLOYED)
  {
    Serial.println("\r\nDEP-RESET failure!");
    Serial.println("Please check the deploy switch!");
  }
  else
  {
    Serial.println("\r\nDEP-RESET success!");
    Serial.println("Reflash firmware with SATELLITE_RESET_ANTENNA_STATUS 0.");
  }

  while (1)
  {
  }
#endif
}

/**
 * @brief Initialize the peripherals on OBC.
 * @note Payload must before any other I2C devices.
 */
void satellite::init(void)
{
  preinitialize_eeprom_params();
  watchdog::init();
  watchdog::set_time_period(_3_MINUTES);

  // First time after being deployed to space
  if (!eeprom::read(SATELLITE_EEPROM_BIRTH_FLAG))
  {
    for (uint8_t i = 0; i < SATELLITE_TIME_WAIT_AFTER_LAUNCH; i++)
    {
      delay(30000);
      watchdog::toggle();
      delay(30000);
      watchdog::toggle();
    }
  }

  payload::init();
  watchdog::toggle();
  antenna::init();

  watchdog::toggle();

  rf4463.init();
  rf4463.enterStandbyMode();

  watchdog::toggle();
}

/**
 * @brief Reset the OBC by setting Watchdog timer to 1 sec and infinite loop.
 */
void satellite::restart(void)
{
  // Set intentional reset mode
  reset::set_status(ResetStatus::INTENTIONAL);
  watchdog::set_time_period(_1_SECOND);
  satellite::enter_sleep_mode();

  while (1)
  {
  }
}

/**
 * @brief Reset the OBC by setting Watchdog timer to 3 mins and infinite loop.
 */
void satellite::restart_3mins(void)
{
  // Set intentional reset mode
  reset::set_status(ResetStatus::INTENTIONAL);
  watchdog::set_time_period(_3_MINUTES);
  satellite::enter_sleep_mode();

  while (1)
  {
  }
}

/**
 * @brief Returns the power mode based on the battery voltage
 * @details
 *    Mode              Min voltage                     Max voltage
 *    ====              ===========                     ===========
 *    FATAL      0                               SATELLITE_POWER_MODE_FATAL
 *    CRITICAL   SATELLITE_POWER_MODE_FATAL      SATELLITE_POWER_MODE_CRITICAL
 *    MEDIUM     SATELLITE_POWER_MODE_CRITICAL   SATELLITE_POWER_MODE_MEDIUM
 *    NORMAL     SATELLITE_POWER_MODE_LOW        SATELLITE_POWER_MODE_NORMAL
 *
 * @return SatellitePowerMode
 */
SatellitePowerMode satellite::get_power_mode(void)
{
  sensors::multimeter_init();
  float battery_voltage = sensors::multimeter_get_voltage();
  sensors::multimeter_save_power();

  SatellitePowerMode pm = CRITICAL;
  if (battery_voltage < SATELLITE_POWER_MODE_FATAL)
  {
    pm = SatellitePowerMode::FATAL;
  }
  else if (battery_voltage > SATELLITE_POWER_MODE_FATAL &&
           battery_voltage < SATELLITE_POWER_MODE_CRITICAL)
  {
    pm = SatellitePowerMode::CRITICAL;
  }
  else if (battery_voltage > SATELLITE_POWER_MODE_CRITICAL &&
           battery_voltage < SATELLITE_POWER_MODE_MEDIUM)
  {
    pm = SatellitePowerMode::MEDIUM;
  }
  else if ((battery_voltage > SATELLITE_POWER_MODE_MEDIUM &&
            battery_voltage < SATELLITE_POWER_MODE_NORMAL))
  {
    pm = SatellitePowerMode::NORMAL;
  }

  return pm;
}

void satellite::enter_sleep_mode(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
}

/**
 * @brief Construct CW Tx string package from SatelliteHouseKeepingPacket.
 *
 * @param pkt Struct containing the satellite housekeeping data.
 * @return String Comma seperated SatelliteHouseKeepingPacket.
 */
String satellite::cw_create_frame(const struct SatelliteHouseKeepingPacket pkt)
{
  const char comma = ',';
  String morse_message;
  morse_message.concat(pkt.call_sign);
  morse_message.concat(comma);
  morse_message.concat(String(pkt.bat_voltage));
  morse_message.concat(comma);
  morse_message.concat(String(pkt.bat_current));
  morse_message.concat(comma);
  morse_message.concat(String(pkt.reset_counts));
  morse_message.concat(comma);
  morse_message.concat(String(pkt.antenna_status));
  morse_message.concat(comma);
  morse_message.concat(String(pkt.payload_status));
  
  return morse_message;
}

/**
 * @brief Increase the reset count in EEPROM.
 * @note Only increase if the reset was not intentional.
 */
void satellite::handle_reset(void)
{
  ResetStatus reset_status = reset::get_status();

  if (reset_status == ResetStatus::UNINTENTIONAL)
  {
    reset::inc_count();                                     // Increase the reset count
    eeprom::write(SATELLITE_EEPROM_TX_IMAGE_FLAG, 1);       // Set image transmission flag
    eeprom::write(SATELLITE_EEPROM_RESET_DELAY_COUNTER, 0); // No extra timing delays
  }
#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
  else
  {
    Serial.println("Intentional reset");
  }
#endif

  reset::set_status(ResetStatus::UNINTENTIONAL); // Unintentional reset for next time
}
