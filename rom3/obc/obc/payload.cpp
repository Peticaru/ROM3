#include <Wire.h>
#include "payload.h"
#include "watchdog.h"
#include "communication.h"
#include "satellite_config.h"
#include "RF4463.h"

extern RF4463 rf4463;

uint8_t ssdv_packet[128] = {0};
volatile uint16_t ssdv_index = 0;

void payload::init(void)
{
  payload::reset();
  delay(5000);
}

void payload::reset(void)
{
  Wire.begin();
  payload::send_command(SATELLITE_PAYLOAD_I2C_ADD, PAYLOAD_I2C_RESET);
  Wire.end();
}

void payload::fetch_and_transmit_image(PayloadImage pi)
{
  Wire.begin();
  payload::send_command(SATELLITE_PAYLOAD_I2C_ADD, PAYLOAD_I2C_REFRESH);
  if(pi == STATIC) // Get test card
  {
    payload::send_command(SATELLITE_PAYLOAD_I2C_ADD, PAYLOAD_I2C_CAPTURE_IMAGE);  
  }
  else if (pi == CAMERA) // Take image
  {
    payload::send_command(SATELLITE_PAYLOAD_I2C_ADD, PAYLOAD_I2C_CAPTURE_IMAGE);  
  }
  else
  {
    return;
  }
  Wire.end();

  // Wait
  for (uint8_t i = 0; i < SATELLITE_PAYLOAD_WAIT_IMAGE_PROCESSING; i++)
  {
    Serial.print('!');
    delay(1000);
  }
  Serial.println();

  // Gets image
  Wire.begin();
  TWCR = 0;
  payload::request_packet();
}

void payload::request_packet(void)
{
  uint8_t flag = 0;
  uint8_t number_of_packets = 0;
  payload::send_command(SATELLITE_PAYLOAD_I2C_ADD, PAYLOAD_I2C_GET_SSDV_CNT);
  
  bool communication_flag = false; 
  Wire.requestFrom(SATELLITE_PAYLOAD_I2C_ADD, 1);
  watchdog::set_time_period(_1_SECOND);
  while (Wire.available())
  {
    watchdog::toggle();
    watchdog::set_time_period(_3_MINUTES);
    communication_flag = true;
    
    number_of_packets = uint8_t(Wire.read());
  }
  watchdog::set_time_period(_3_MINUTES);

  Serial.print("Packets: ");
  Serial.println(number_of_packets);

  if(number_of_packets > PAYLOAD_I2C_MAX_PACKETS || number_of_packets == 0)
  {
    communication_flag = false;
    number_of_packets = 0;
  }

  if(communication_flag == false)
  {
    return;
  }

  payload::send_command(SATELLITE_PAYLOAD_I2C_ADD, PAYLOAD_I2C_REFRESH);

  // SSDV Packet loop
  for (uint8_t i = 0; i < number_of_packets; i++)
  {
    payload::send_command(SATELLITE_PAYLOAD_I2C_ADD, PAYLOAD_I2C_FETCH_SSDV_FLASH);
    
    // Mini Packet loop
    for (uint8_t j = 0; j < 8; j++)
    {
      // Ask for 32 SSDV bytes
      watchdog::toggle();
      
      Wire.requestFrom(SATELLITE_PAYLOAD_I2C_ADD, PAYLOAD_I2C_MAX_INPUT_BYTES);

#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
      Serial.print("/*");
      Serial.print(i);
      Serial.print('.');
      Serial.print(j);
      Serial.print("*/ ");
#endif

      watchdog::set_time_period(_1_SECOND);
      while (Wire.available())
      {
        watchdog::toggle();
        watchdog::set_time_period(_3_MINUTES);
        ssdv_packet[ssdv_index] = uint8_t(Wire.read());
        
#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
        Serial.print("0x");
        Serial.print(ssdv_packet[ssdv_index], HEX);
        Serial.print(", ");
        flag = 1;
#endif

        ssdv_index++;
      }
      watchdog::set_time_period(_3_MINUTES);

#if SATELLITE_ENABLE_SERIAL_DEBUG == 1
      if (flag)
      {
        Serial.println();
      }
      flag = 0;
#endif

        watchdog::toggle();
        uint8_t status_ = payload::transmit_ssdv_pkt();
        watchdog::toggle();
        ssdv_index = 0;
      
      payload::send_command(SATELLITE_PAYLOAD_I2C_ADD, PAYLOAD_I2C_UPDATE_MINI_PKT);
    }
    ssdv_index = 0;
    payload::send_command(SATELLITE_PAYLOAD_I2C_ADD, PAYLOAD_I2C_UPDATE_SSDV_PKT);
  }
  rf4463.enterStandbyMode();
}

void payload::send_command(uint8_t add, uint8_t cmd)
{
  Wire.beginTransmission(add);
  Wire.write(cmd);
  Wire.endTransmission();
}

uint8_t payload::transmit_ssdv_pkt(void)
{
  bool tx_status = rf4463.txPacket(ssdv_packet, 64);
  delay(PACKET_DELAY);
  return tx_status;
}
