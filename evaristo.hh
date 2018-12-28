#ifndef EVARISTO_HH
#define EVARISTO_HH
#include <ostream>
#include <memory>

#define EVARISTO_VERSION 200 // major * 100 + minor

struct evaristo {
  evaristo() {
    marker = 0xFFFF;
    header_length = sizeof(evaristo) / sizeof(uint16_t);
    version = EVARISTO_VERSION;
  }
  uint16_t marker, header_length;  // length in samples
  uint32_t counter, time_tag, n_samples, cpu_time_ms;
  uint16_t n_channels, version; 
  uint32_t data_length, unused[3];
  struct channel_data {
    uint16_t channel, n_samples, unused[2];
    uint16_t samples[0];
  } data[0];
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
