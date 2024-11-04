// 2022/09/19

#include "eeprom.h"
#include "satellite_config.h"
#include "communication.h"
#include "telecommand.h"
#include "satellite.h"
#include "antenna.h"

void telecommand::init(void)
{
  communication::gfsk_rx_init();
}

TelecommandFunction telecommand::listen_to_ground(void)
{
  TelecommandFunction tf = TelecommandFunction::PASS;
  uint8_t rx_buff[SATELLITE_TELECOMMAND_BUFF_LEN];
  uint8_t rx_len = SATELLITE_TELECOMMAND_BUFF_LEN;

  uint32_t listen_timekeeper = millis();
  bool wait_flag = true;

  while ((millis() - listen_timekeeper < SATELLITE_TIME_LISTEN_FOR) && wait_flag)
  {
    if (radio.recv(rx_buff, &rx_len))
    {
#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
      Serial.print("Cmd: ");
      Serial.println((char *)rx_buff);
#endif
      uint8_t override[] = SATELLITE_TELECOMMAND_PRE_OVERRIDE;
      uint8_t acknowledge[] = SATELLITE_TELECOMMAND_PRE_HI_ROM2;

      /* Reception of override preamble */
      if (rx_buff[0] == override[0] && rx_buff[1] == override[1])
      {
        wait_flag = false;
        switch (rx_buff[2])
        {
        case SATELLITE_TELECOMMAND_DEP_ANT:
        {
          tf = TelecommandFunction::DEP_ANT;
          break;
        }
        case SATELLITE_TELECOMMAND_STOP_COMM:
        {
          tf = TelecommandFunction::STOP_COMM;
          break;
        }
        case SATELLITE_TELECOMMAND_START_COMM:
        {
          tf = TelecommandFunction::START_COMM;
          break;
        }
        case SATELLITE_TELECOMMAND_STOP_IMG:
        {
          tf = TelecommandFunction::STOP_IMG;
          break;
        }
        case SATELLITE_TELECOMMAND_START_IMG:
        {
          tf = TelecommandFunction::START_IMG;
          break;
        }
        case SATELLITE_TELECOMMAND_STOP_GFSK:
        {
          tf = TelecommandFunction::STOP_GFSK;
          break;
        }
        case SATELLITE_TELECOMMAND_START_GFSK:
        {
          tf = TelecommandFunction::START_GFSK;
          break;
        }
        case SATELLITE_TELECOMMAND_STOP_CW:
        {
          tf = TelecommandFunction::STOP_CW;
          break;
        }
        case SATELLITE_TELECOMMAND_START_CW:
        {
          tf = TelecommandFunction::START_CW;
          break;
        }
        case SATELLITE_TELECOMMAND_IMG_GFSK_DT:
        {
          tf = TelecommandFunction::IMG_GFSK_DT;
          break;
        }
        case SATELLITE_TELECOMMAND_GFSK_CW_DT:
        {
          tf = TelecommandFunction::GFSK_CW_DT;
          break;
        }
        case SATELLITE_TELECOMMAND_CW_HEAR_DT:
        {
          tf = TelecommandFunction::CW_HEAR_DT;
          break;
        }
        default:
        {
          tf = TelecommandFunction::PASS;
          break;
        }
        }
      }
      /* Reception of acknowledgement preamble */
      else if (rx_buff[0] == acknowledge[0] &&
               rx_buff[1] == acknowledge[1] &&
               rx_buff[2] == acknowledge[2])
      {
        wait_flag = false;
        tf = TelecommandFunction::HI_ROM2;
      }
    }
  }

#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
  telecommand::print_function(tf);
#endif

  return tf;
}

void telecommand::execute_function(enum TelecommandFunction t)
{
  switch (t)
  {
  case HI_ROM2:
  {
    break;
  }
  case DEP_ANT:
  {
    eeprom::write(SATELLITE_EEPROM_BIRTH_FLAG, 0);
    antenna::deploy();
    break;
  }
  case START_COMM:
  {
    eeprom::write(SATELLITE_EEPROM_TX_IMAGE_SW, 1);
    eeprom::write(SATELLITE_EEPROM_TX_GFSK_SW, 1);
    eeprom::write(SATELLITE_EEPROM_TX_CW_SW, 1);
    break;
  }
  case STOP_COMM:
  {
    eeprom::write(SATELLITE_EEPROM_TX_IMAGE_SW, 0);
    eeprom::write(SATELLITE_EEPROM_TX_GFSK_SW, 0);
    eeprom::write(SATELLITE_EEPROM_TX_CW_SW, 0);
    break;
  }
  case STOP_IMG:
  {
    eeprom::write(SATELLITE_EEPROM_TX_IMAGE_SW, 0);
    break;
  }
  case START_IMG:
  {
    eeprom::write(SATELLITE_EEPROM_TX_IMAGE_SW, 1);
    break;
  }
  case STOP_GFSK:
  {
    eeprom::write(SATELLITE_EEPROM_TX_GFSK_SW, 0);
    break;
  }
  case START_GFSK:
  {
    eeprom::write(SATELLITE_EEPROM_TX_GFSK_SW, 1);
    break;
  }
  case STOP_CW:
  {
    eeprom::write(SATELLITE_EEPROM_TX_CW_SW, 0);
    break;
  }
  case START_CW:
  {
    eeprom::write(SATELLITE_EEPROM_TX_CW_SW, 1);
    break;
  }
  case IMG_GFSK_DT:
  {
    break;
  }
  case GFSK_CW_DT:
  {
    break;
  }
  case CW_HEAR_DT:
  {
    break;
  }
  default:
  {
    break;
  }
  }
}

#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
void telecommand::print_function(enum TelecommandFunction t)
{
  if (t != PASS)
  {
    Serial.print("Rx: ");
    switch (t)
    {
    case HI_ROM2:
    {
      Serial.println("HI_ROM2");
      break;
    }
    case DEP_ANT:
    {
      Serial.println("DEP_ANT");
      break;
    }
    case START_COMM:
    {
      Serial.println("START_COMM");
      break;
    }
    case STOP_COMM:
    {
      Serial.println("STOP_COMM");
      break;
    }
    case STOP_IMG:
    {
      Serial.println("STOP_IMG");
      break;
    }
    case START_IMG:
    {
      Serial.println("START_IMG");
      break;
    }
    case STOP_GFSK:
    {
      Serial.println("STOP_GFSK");
      break;
    }
    case START_GFSK:
    {
      Serial.println("START_GFSK");
      break;
    }
    case STOP_CW:
    {
      Serial.println("STOP_CW");
      break;
    }
    case START_CW:
    {
      Serial.println("START_CW");
      break;
    }
    case IMG_GFSK_DT:
    {
      Serial.println("IMG_GFSK_DT");
      break;
    }
    case GFSK_CW_DT:
    {
      Serial.println("GFSK_CW_DT");
      break;
    }
    case CW_HEAR_DT:
    {
      Serial.println("CW_HEAR_DT");
      break;
    }
    default:
    {
      break;
    }
    }
  }
}
#endif
