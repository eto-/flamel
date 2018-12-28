#include <algorithm>
#include <iterator>
#include <iostream>
#include "evaristo.hh"

std::ostream & operator << (std::ostream &o, const evaristo &e) {
  o << "data_length: " << e.data_length << std::endl;
  o << "counter: " << e.counter << std::endl;
  o << "time_tag: " << e.time_tag << std::endl;
  o << "cpu_time_ms: " << e.cpu_time_ms << std::endl;  
  o << "version: " << e.version << std::endl;
  o << "n_channels: " << e.n_channels << std::endl;
  //o << "samples" << std::endl;
  const uint16_t * ptr = reinterpret_cast<const uint16_t*>(e.data);
  for (int k = 0; k < e.n_channels; k++) {
    const evaristo::channel_data &d = *reinterpret_cast<const evaristo::channel_data*>(ptr);
    o << "ch: " << d.channel << std::endl;
    o << "n_samples: " << d.n_samples << std::endl;
    o << d.samples[0];
    for (int i = 1; i < d.n_samples; i++) o << "       " << d.samples[i];
    o << std::endl;

    ptr += sizeof(evaristo::channel_data)/sizeof(uint16_t) + d.n_samples;
  }
  o << std::endl;

  return o;
}

std::ostream & operator << (std::ostream& o, const metadata& m) {
  o << "board: " << m.board << std::endl;
  o << "n_bits: " << m.n_bits << std::endl;
  o << "sampling_rate: " << m.sampling_rate << std::endl;
  o << "threshold: " << m.threshold << std::endl;
  o << std::endl;

  return o;
}


