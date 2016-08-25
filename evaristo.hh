#ifndef EVARISTO_HH
#define EVARISTO_HH
#include <ostream>

struct evaristo {
  u_int16_t marker, header_length;
  u_int32_t counter, time_tag, n_samples, cpu_time_ms, n_channels, unused[4];
  u_int16_t samples[0];
};

std::ostream & operator << (std::ostream&, const evaristo&);

struct metadata {
  int board;
  int n_bits;
  int sampling_rate;
  int threshold;
};

std::ostream & operator << (std::ostream&, const metadata&);

 
#endif
