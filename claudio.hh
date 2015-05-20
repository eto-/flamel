#ifndef claudio_hh
#define claudio_hh

#include <string>
#include <vector>

struct claudio_ev {
  u_int32_t size, time_tag;
  u_int16_t samples[0];
};
 
class claudio {
  public:
    claudio ();
    ~claudio ();

    void start ();
    void stop ();
    std::vector<claudio_ev*> loop ();
  private:
    int handle_;
    char *event_buffer_;
    uint32_t buffer_size_;
    void *decoded_event_;


    void init_link ();
    void init_channels ();
    void init_trigger ();
    void init_buffers ();
    void close_link ();

    uint32_t get_register (uint16_t reg); 
    void set_register (uint16_t reg, uint32_t value);
    uint32_t set_register_bits (uint16_t reg, uint32_t bits);
    bool wait_irq ();
};
#endif
