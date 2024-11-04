// Interface to the satellite mission based functions
// 2022/07/13

#ifndef _SATELLITE_H_
#define _SATELLITE_H_

#include "Arduino.h"
#include "satellite_config.h"

struct SatelliteHouseKeepingPacket
{
  String call_sign;
  float obc_temp;
  float bat_voltage;
  float bat_current;
  uint8_t reset_counts;
  bool antenna_status;
  bool payload_status;
};

enum SatellitePowerMode
{
  FATAL,    // 0                             - SATELLITE_POWER_MODE_FATAL
  CRITICAL, // SATELLITE_POWER_MODE_FATAL    - SATELLITE_POWER_MODE_CRITICAL
  MEDIUM,   // SATELLITE_POWER_MODE_CRITICAL - SATELLITE_POWER_MODE_MEDIUM
  NORMAL    // SATELLITE_POWER_MODE_LOW      - SATELLITE_POWER_MODE_NORMAL
};

namespace satellite
{
  // Satellite system functions
  void init(void);
  SatellitePowerMode get_power_mode(void);
  void enter_sleep_mode(void);
  void restart(void);
  void restart_3mins(void);

  // Communication functions
  String cw_create_frame(const struct SatelliteHouseKeepingPacket pkt);

  // Reset counter and handlers
  void handle_reset(void);
}

#endif // satellite.h
