// Germain (Saint) manages the alchemy of the communications with the CAEN V2740 digitizer
//   - init initialize the digitizer
//   - star & stop manage the run
//   - loop fetches new data from the digitizer
//   - info returns metadata
//   - loop fetches new data from the digitizer
//   - info returns metadata
#ifndef GERMAIN_HH
#define GERMAIN_HH

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "evaristo.hh"

class germain {
  public:
    germain ();
    ~germain ();

    void init ();
    void start ();
    void stop ();

    std::vector<std::unique_ptr<evaristo>> loop ();

    const aristotele& info () const { return aristotele_; }

  private:
    uint64_t handle_;
    aristotele aristotele_;
    /*
    char *event_buffer_;
    uint32_t buffer_size_;
    void *decoded_event_;
    bool sw_trigger_,wait_irq_;
    std::chrono::time_point<std::chrono::system_clock> start_time_; 

    */

    void init_link ();
    void init_channels ();
    void init_trigger ();
    void init_buffers ();
    void init_metadata ();
    void close_link ();
};
#endif
