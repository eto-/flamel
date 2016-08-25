#include <algorithm>
#include <iterator>
#include <iostream>
#include "evaristo.hh"

std::ostream & operator << (std::ostream &o, const evaristo &e) {
  o << "n_samples: " << e.n_samples << std::endl;
  o << "counter: " << e.counter << std::endl;
  o << "time_tag: " << e.time_tag << std::endl;
  o << "cpu_time_ms: " << e.cpu_time_ms << std::endl;  
  //o << "samples" << std::endl;
  for (int i = 0; i < e.n_samples; i++) {
    o << e.samples[i];
    for (int k = 1; k < e.n_channels; k++) o << "	" << e.samples[i + k * e.n_samples];
    o << std::endl;
  }

  return o;
}

std::ostream & operator << (std::ostream& o, const metadata& m) {
  o << "board: " << m.board << std::endl;
  o << "n_bits: " << m.n_bits << std::endl;
  o << "sampling_rate: " << m.sampling_rate << std::endl;
  o << "threshold: " << m.threshold << std::endl;
}


