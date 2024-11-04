#ifndef _SENSORS_H_
#define _SENSORS_H_

namespace sensors
{
  // Current sensor
  void multimeter_init(void);
  float multimeter_get_current(void);
  float multimeter_get_voltage(void);
  float multimeter_save_power(void);
}

#endif // sensors.h
