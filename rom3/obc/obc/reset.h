// 2022-10-13

#ifndef _RESET_H_
#define _RESET_H_

#include <inttypes.h>

enum ResetStatus
{
  UNINTENTIONAL,
  INTENTIONAL
};

namespace reset
{
  uint8_t get_count(void);
  void clear_count(void);
  void inc_count(void);
  void dec_count(void);

  ResetStatus get_status(void);
  void set_status(ResetStatus rs);
}

#endif // reset.h
