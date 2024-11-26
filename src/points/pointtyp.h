#ifndef POINTTYP_H_
#define POINTTYP_H_
#include <Arduino.h>


enum pointTyp : int8_t
{
  PT_TEMP = 0,
  PT_TEMPT = 1,
  PT_LOGIC = 2,
  PT_OUT = 3,
  PT_MIXER = 4
};

#endif /* POINTTYP_H_ */