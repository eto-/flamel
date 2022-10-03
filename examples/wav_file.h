#ifndef WAV_FILE_H
#define WAV_FILE_H
#include <sndfile.hh>
#include <vector>
#include <iostream>
#include <string>

class wav_file {
  public:
    wav_file(const std::string& fname);

    ssize_t length() const { return _length; }
    int n_samples() const { return _n_sample; }
    int sample_rate() const { return _sample_rate; }
    int bits() const { return _bits; }

    const std::string& date() const { return _date; }

    typedef std::vector<uint16_t> raw_data;
    struct decoded_data {
      uint16_t marker, header_length;
      uint32_t counter, time_tag, n_samples, cpu_time_ms, n_channels, unused[4];
      std::vector<raw_data> data;
    };

    decoded_data ev(ssize_t offset);

    decoded_data next() { return ev(_next_offset); }
    bool eof() { return _next_offset >= _length; }

  private:
    SndfileHandle _file;
    ssize_t _next_offset, _length;
    int _n_sample, _sample_rate, _bits;
    std::string _date;

    
    raw_data read_n(ssize_t offset, ssize_t len);

};

#endif
