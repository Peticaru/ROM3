// 2022-11-03

#ifndef _ANTENNA_H_
#define _ANTENNA_H_

enum AntennaStatus
{
  NOT_DEPLOYED = 0,
  DEPLOYED
};

namespace antenna
{
  void init(void);
  void deploy(void);

  AntennaStatus get_status(void);
  void set_status(const enum AntennaStatus as);
}

#endif // antenna.h