#include "to_7seg.h"

int8_t to_7seg(uint8_t code){
  int8_t const sseg[] = {126, 48, 109, 121, 51, 91, 95, 112, 127, 123, 21};
  if(0 <= code && code <= 10)
    return sseg[code];
  else
    return 0;
}