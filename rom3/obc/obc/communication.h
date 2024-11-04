
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include "RH_RF24.h"
#include <Arduino.h>

extern RH_RF24 radio;

namespace communication
{
  // Comm system functions
  uint8_t init(void);
  void deinit(void);
  void sleep_transmitter(void);
  void wake_transmitter(void);
  float get_temp(void);

  // GFSK image tx functions
  void gfsk_img_init(void);
  void gfsk_tx_init(void);
  void gfsk_rx_init(void);

  // CW housekeeping tx functions
  void cw_init(void);
  String cw_create_frame(struct CommHouseKeepingPacket pkt);
}

#endif // communication.h
