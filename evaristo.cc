#include <algorithm>
#include <iterator>
#include <iostream>
#include "evaristo.hh"

std::ostream & operator << (std::ostream &o, const evaristo &e) {
  o << "n_samples " << e.n_samples << std::endl;
  o << "counter " << e.counter << std::endl;
  o << "time_tag " << e.time_tag << std::endl;
  o << "samples" << std::endl;
  std::copy(e.samples, e.samples + e.n_samples, std::ostream_iterator<u_int16_t>(o, "\n"));

  return o;
}

