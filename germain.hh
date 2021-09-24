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
#include "alchemy.hh"

class germain: public alchemy {
  public:
    germain ();
    virtual ~germain ();

    virtual void init ();
    virtual void start ();
    virtual void stop ();

    virtual std::vector<std::unique_ptr<evaristo>> loop ();

  private:
    uint64_t handle_, scope_;
    bool sw_trigger_, selftrigger_;
    unsigned int board_channels_;
    uint16_t **buffer_;
    size_t *sizes_;
    /*
    char *event_buffer_;
    uint32_t buffer_size_;
    void *decoded_event_;
    std::chrono::time_point<std::chrono::system_clock> start_time_; 

    */

    void init_link ();
    void init_channels ();
    void init_trigger ();
    void init_buffers ();
    void init_metadata ();
    void close_link ();

    void send (const std::string& path);
    template<typename T> T get (const std::string& path);
    template<typename T> void set (const std::string& path, T value) { _set(path, std::to_string(value)); }
    void _set (const std::string& path, const std::string& value);
};
#endif
