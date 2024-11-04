#include "reset.h"
#include "sensor.h"
#include "payload.h"
#include "antenna.h"
#include "watchdog.h"
#include "telecommand.h"
#include "communication.h"
#include "satellite_fsm.h"
#include "eeprom.h"
#include "RF4463.h"

extern RF4463 rf4463;

extern uint8_t ssdv_packet[128];
bool delay_count_dec_flag = false;

SatelliteState satellite_fsm_last_state = SatelliteState::SLEEP;
uint32_t timekeeper = 0;

void fsm::print_state(SatelliteState s);

SatelliteState fsm::get_state(const uint32_t t, const enum SatellitePowerMode m)
{
  watchdog::toggle();

  // SLEEP state by default
  SatelliteState s = SatelliteState::IDLE;

  // Auto reset every SATELLITE_TIME_RESET millis
  if (t > SATELLITE_TIME_RESET)
  {
    return SatelliteState::RESTART;
  }

  // Check delay counter and update it based on the power mode
  // Update the image flag (if necessary) for next image Tx
  uint8_t delay_counter = eeprom::read(SATELLITE_EEPROM_RESET_DELAY_COUNTER);
  if (delay_counter == 0)
  {
    switch (m)
    {
    case NORMAL:
    {
      eeprom::write(SATELLITE_EEPROM_RESET_DELAY_COUNTER, SATELLITE_3MIN_DELAYS_NORMAL);
      eeprom::write(SATELLITE_EEPROM_TX_IMAGE_FLAG, 1); // Enable camera
      break;
    }

    case MEDIUM:
    {
      eeprom::write(SATELLITE_EEPROM_RESET_DELAY_COUNTER, SATELLITE_3MIN_DELAYS_MEDIUM);
      eeprom::write(SATELLITE_EEPROM_TX_IMAGE_FLAG, 1); // Enable camera
      break;
    }

    case CRITICAL:
    {
      eeprom::write(SATELLITE_EEPROM_RESET_DELAY_COUNTER, SATELLITE_3MIN_DELAYS_CRITICAL);
      eeprom::write(SATELLITE_EEPROM_TX_IMAGE_FLAG, 0); // Disable camera
      break;
    }
    }
  }

  // Check if it's time to transmit image
  uint8_t image_tx_flag = eeprom::read(SATELLITE_EEPROM_TX_IMAGE_FLAG);

  // Hush Little Baby
  if (m == FATAL)
  {
    s = SLEEP;
  }

  // Image transmission
  else if (image_tx_flag)
  {
    eeprom::write(SATELLITE_EEPROM_TX_IMAGE_FLAG, 0); // Disable camera
    s = TX_IMAGE;
  }

  // If no image transmission, start with GFSK
  else if (image_tx_flag == 0 & satellite_fsm_last_state == SLEEP)
  {
    s = TX_GFSK;
  }

  // GFSK Tx after image Tx
  else if (satellite_fsm_last_state == TX_IMAGE && millis() - timekeeper > SATELLITE_TIME_IMAGE_TO_GFSK)
  {
    s = TX_GFSK;
  }

  // CW Tx after GFSK Tx
  else if (satellite_fsm_last_state == TX_GFSK && millis() - timekeeper > SATELLITE_TIME_GFSK_TO_CW)
  {
    s = TX_CW;
  }

  // Listen telecommand after CW Tx
  else if (satellite_fsm_last_state == TX_CW && millis() - timekeeper > SATELLITE_TIME_CW_TO_LISTEN)
  {
    s = LISTEN;
  }

  // Sleep and decrease delay after listening to telecommand
  else if (satellite_fsm_last_state == LISTEN)
  {
    delay_counter = eeprom::read(SATELLITE_EEPROM_RESET_DELAY_COUNTER);
    if (delay_counter != 0)
    {
      eeprom::write(SATELLITE_EEPROM_RESET_DELAY_COUNTER, delay_counter - 1);
    }
    s = SLEEP;
  }

  watchdog::toggle();

#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
  fsm::print_state(s);
#endif

  return s;
}

/*
  Inputs:
  t = Time period from last reset
  dt = Time period from last function call

  Output:
  Perform state specific task and update to new state
*/
void fsm::execute_state(const enum SatelliteState s)
{
  watchdog::toggle();

  switch (s)
  {
  case SLEEP:
  {
    fsm::execute_state_sleep();
    break;
  }
  case RESTART:
  {
    satellite::restart();
    break;
  }
  case TX_CW:
  {
    if (eeprom::read(SATELLITE_EEPROM_TX_CW_SW) == 1)
    {
      fsm::execute_state_tx_gfsk();
    }
    break;
  }
  case TX_IMAGE:
  {
    if (eeprom::read(SATELLITE_EEPROM_TX_IMAGE_SW) == 1)
    {
      fsm::execute_state_tx_image();
    }
    break;
  }
  case TX_GFSK:
  {
    if (eeprom::read(SATELLITE_EEPROM_TX_GFSK_SW) == 1)
    {
      fsm::execute_state_tx_gfsk();
    }
    break;
  }
  }

  // Sorry IDLE! We don't conside you as state
  if (s != SatelliteState::IDLE)
  {
    timekeeper = millis(); // Reset the timekeeper
    satellite_fsm_last_state = s;
  }

  watchdog::toggle();
}

void fsm::execute_state_sleep(void)
{
  satellite::restart_3mins();
}



void fsm::execute_state_tx_image(void)
{
  bool image_type = eeprom::read(SATELLITE_EEPROM_IMAGE_TOGGLE);

  if((int)image_type == CAMERA)
  {
    payload::fetch_and_transmit_image(CAMERA);
    eeprom::write(SATELLITE_EEPROM_IMAGE_TOGGLE, STATIC);  
  }
  else if((int)image_type == STATIC)
  {
    payload::fetch_and_transmit_image(STATIC);
    eeprom::write(SATELLITE_EEPROM_IMAGE_TOGGLE, CAMERA);
  }
}

void fsm::execute_state_tx_gfsk(void) {
  // communication::init();
  sensors::multimeter_init();

  SatelliteHouseKeepingPacket morse_pkt;
  morse_pkt.call_sign = SATELLITE_TX_CALLSIGN;
  morse_pkt.bat_voltage = sensors::multimeter_get_voltage();
  morse_pkt.bat_current = sensors::multimeter_get_current();
  morse_pkt.reset_counts = reset::get_count();
  morse_pkt.antenna_status = antenna::get_status();
  morse_pkt.payload_status = 0;

  sensors::multimeter_save_power();

/*
#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
  Serial.print("T:");
  Serial.print(morse_pkt.obc_temp);
  Serial.print(", V:");
  Serial.print(morse_pkt.bat_voltage);
  Serial.print(", I:");
  Serial.print(morse_pkt.bat_current);
  Serial.print(", R:");
  Serial.print(morse_pkt.reset_counts);
  Serial.print(", A:");
  Serial.print(morse_pkt.antenna_status);
  Serial.print(", P:");
  Serial.println(morse_pkt.payload_status);
#endif
*/


  String tx_frame = satellite::cw_create_frame(morse_pkt);
  unsigned char tx_frame_char[80];
  tx_frame.toCharArray(tx_frame_char, tx_frame.length() + 1);
  
  Serial.println(tx_frame);
  Serial.print("Of length");
  Serial.println(tx_frame.length());
  
  rf4463.txPacket(tx_frame_char,sizeof(tx_frame_char));
  rf4463.enterStandbyMode();
  
  delay(3000);
         

#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
  Serial.println();
#endif
}


#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
void fsm::print_state(SatelliteState s)
{
  switch (s)
  {
  case SLEEP:
  {
    Serial.println("SLEEP");
    Serial.println();
    break;
  }

  case RESTART:
  {
    Serial.println("RESTART");
    break;
  }

  case TX_CW:
  {
    if (eeprom::read(SATELLITE_EEPROM_TX_CW_SW) == 1)
    {
      Serial.println("CW");
    }
    break;
  }

  case TX_IMAGE:
  {
    if (eeprom::read(SATELLITE_EEPROM_TX_IMAGE_SW) == 1)
    {
      Serial.println("IMAGE");
    }
    break;
  }

  case TX_GFSK:
  {
    if (eeprom::read(SATELLITE_EEPROM_TX_GFSK_SW) == 1)
    {
      Serial.println("GFSK");
    }
    break;
  }

  case LISTEN:
  {
    Serial.println("LISTEN");
    break;
  }
  }
}
#endif
