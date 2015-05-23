#ifndef claudio_hh
#define claudio_hh

#include <string>
#include <vector>
#include <memory>
#include "evaristo.hh"

class claudio {
  public:
    claudio ();
    ~claudio ();

    std::string init ();
    void start ();
    void stop ();

    std::vector<std::unique_ptr<evaristo>> loop ();

  private:
    bool emulate_hw_;
    int handle_;
    char *event_buffer_;
    uint32_t buffer_size_;
    void *decoded_event_;


    void init_link ();
    void init_channels ();
    void init_trigger ();
    void init_buffers ();
    void close_link ();

    std::vector<std::unique_ptr<evaristo>> emulate_loop ();

    uint32_t get_register (uint16_t reg); 
    void set_register (uint16_t reg, uint32_t value);
    uint32_t set_register_bits (uint16_t reg, uint32_t bits);
    bool wait_irq ();
};
#endif
