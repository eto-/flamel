// Evaristo is the event storing the waveform and other informations
// Aristotele contains the run metadata
#ifndef EVARISTO_HH
#define EVARISTO_HH
#include <ostream>
#include <memory>

struct evaristo {
  uint16_t marker, header_length;
  uint32_t counter, time_tag, n_samples, cpu_time_ms, n_channels, unused[4];
  uint16_t samples[0];
};

std::ostream & operator << (std::ostream&, const evaristo&);

struct aristotele {
  int board;
  int n_bits;
  int sampling_rate;
  int threshold;
  int gate_length;
  int post_trigger;
};

std::ostream & operator << (std::ostream&, const aristotele&);

 
#endif
