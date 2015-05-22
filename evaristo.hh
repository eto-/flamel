#ifndef EVARISTO_HH
#define EVARISTO_HH

struct evaristo {
  u_int32_t n_samples, counter, time_tag;
  u_int16_t samples[0];
};
 
#endif
