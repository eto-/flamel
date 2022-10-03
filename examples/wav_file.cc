#include "wav_file.h"
#include <cstring>

wav_file::wav_file(const std::string& fname): _file(fname.c_str()), _next_offset(0) {
  _sample_rate = _file.samplerate ();
  _bits = 16;
  _date = _file.getString (SF_STR_DATE);
  _length = _file.frames();
}

wav_file::raw_data wav_file::read_n (ssize_t offset, ssize_t len) {
  _file.seek (offset, SEEK_SET);

  raw_data v(len);
  sf_count_t c = _file.read ((short *)&v[0], len);

  if (c != len) {
    std::cerr << "error: " << _file.strError() << std::endl;
    return raw_data{0};
  }

  return v;
}

wav_file::decoded_data wav_file::ev (ssize_t offset) {
  const size_t header_length = 20;
  const raw_data& r = read_n(offset, header_length * 2);

  decoded_data d, *h = (decoded_data*)&r[0];
  if (h->marker != 0xFFFF) {
    std::cerr << "error: missing marker" << std::endl;
    return d;
  }
  if (h->header_length != header_length) {
    std::cerr << "error: wrong header length" << std::endl;
    return d;
  }

  std::memcpy(&d, h, header_length * 2);
  
  const raw_data& rd = read_n (offset + header_length, d.n_samples * d.n_channels);
  d.data.resize(d.n_channels);
  for (ssize_t i = 0; i < d.n_channels; i++) {
    d.data[i].reserve(d.n_samples);
    std::memcpy(&(d.data[i][0]), &rd[i*d.n_samples], d.n_samples * 2);
  }

  _next_offset += h->header_length + d.n_samples * d.n_channels;

  return d;
}
