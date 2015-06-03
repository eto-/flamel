#include <algorithm>
#include <iterator>
#include <iostream>
#include "evaristo.hh"

std::ostream & operator << (std::ostream &o, const evaristo &e) {
  o << "n_samples: " << e.n_samples << std::endl;
  o << "counter: " << e.counter << std::endl;
  o << "time_tag: " << e.time_tag << std::endl;
  //o << "samples" << std::endl;
  std::copy(e.samples, e.samples + e.n_samples, std::ostream_iterator<u_int16_t>(o, "\n"));

  return o;
}

std::ostream & operator << (std::ostream& o, const metadata& m) {
  o << "board: " << m.board << std::endl;
  o << "n_bits: " << m.n_bits << std::endl;
  o << "sampling_rate: " << m.sampling_rate << std::endl;
}


