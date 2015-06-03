#ifndef EVARISTO_HH
#define EVARISTO_HH
#include <ostream>

struct evaristo {
  u_int32_t counter, time_tag, n_samples;
  u_int16_t samples[0];
};

std::ostream & operator << (std::ostream&, const evaristo&);

struct metadata {
  int board;
  int n_bits;
  int sampling_rate;
};

std::ostream & operator << (std::ostream&, const metadata&);

 
#endif
