#ifndef _PAYLOAD_H_
#define _PAYLOAD_H_

#include <inttypes.h>

extern uint8_t ssdv_packet[128];

// I2C command bytes and functions
#define PAYLOAD_I2C_RESET 0x7e
#define PAYLOAD_I2C_REFRESH 0x15
#define PAYLOAD_I2C_CAPTURE_IMAGE 0x25
#define PAYLOAD_I2C_UPDATE_SSDV_PKT 0x35
#define PAYLOAD_I2C_UPDATE_MINI_PKT 0x45
#define PAYLOAD_I2C_FETCH_SSDV_FLASH 0x55
#define PAYLOAD_I2C_GET_SSDV_CNT 0x75
#define PAYLOAD_I2C_STATIC_IMAGE 0x85

#define PAYLOAD_I2C_MAX_INPUT_BYTES 32
#define PAYLOAD_I2C_MAX_PACKETS 60

enum PayloadImage
{
  STATIC = 0, // Static image stored in flash
  CAMERA = 1 // Image from camera
};

namespace payload
{
  void init(void);
  void reset(void);
  void request_packet(void);
  uint8_t transmit_ssdv_pkt(void);
  void fetch_and_transmit_image(const PayloadImage pi);
  void send_command(uint8_t add, uint8_t cmd);
}

#endif // payload.h
