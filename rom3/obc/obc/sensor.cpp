
#include <Arduino.h>
#include <Adafruit_INA219.h>
#include "sensor.h"

Adafruit_INA219 current_sensor;

void sensors::multimeter_init(void)
{
  current_sensor.begin();
  current_sensor.setCalibration_16V_400mA();
}

float sensors::multimeter_get_current(void)
{
  return current_sensor.getCurrent_mA();
}

float sensors::multimeter_get_voltage(void)
{
  return (current_sensor.getBusVoltage_V());
}

float sensors::multimeter_save_power(void)
{
  current_sensor.powerSave(1);
}
