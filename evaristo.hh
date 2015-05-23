#ifndef EVARISTO_HH
#define EVARISTO_HH
#include <ostream>

struct evaristo {
  u_int32_t n_samples, counter, time_tag;
  u_int16_t samples[0];
};

std::ostream & operator << (std::ostream&, const evaristo&);
 
#endif
