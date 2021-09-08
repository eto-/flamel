// Flamel manages the alchemy of the communications with the CAEN digitizer
//   - init initialize the digitizer
//   - star & stop manage the run
//   - loop fetches new data from the digitizer
//   - info returns metadata
//   - loop fetches new data from the digitizer
//   - info returns metadata
#ifndef FLAMEL_HH
#define FLAMEL_HH

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "evaristo.hh"

class flamel {
  public:
    flamel ();
    ~flamel ();

    void init ();
    void start ();
    void stop ();

    std::vector<std::unique_ptr<evaristo>> loop ();

    const aristotele& info () const { return aristotele_; }

  private:
    bool emulate_hw_;
    int handle_;
    char *event_buffer_;
    uint32_t buffer_size_;
    void *decoded_event_;
    bool sw_trigger_,wait_irq_;
    aristotele aristotele_;
    std::chrono::time_point<std::chrono::system_clock> start_time_; 


    void init_link ();
    void init_channels ();
    void init_trigger ();
    void init_buffers ();
    void init_metadata ();
    void close_link ();

    std::vector<std::unique_ptr<evaristo>> emulate_loop ();

    uint32_t get_register (uint16_t reg); 
    void set_register (uint16_t reg, uint32_t value);
    uint32_t set_register_bits (uint16_t reg, uint32_t bits);
    uint32_t clear_register_bits (uint16_t reg, uint32_t bits);
    bool wait_irq ();
};
#endif
