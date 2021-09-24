// alchemy is the base daq class
//   - init initialize the digitizer
//   - star & stop manage the run
//   - loop fetches new data from the digitizer
//   - info returns metadata
#ifndef ALCHEMY_HH
#define ALCHEMY_HH

#include <vector>
#include <memory>
#include <chrono>
#include "evaristo.hh"

class alchemy {
  public:
    alchemy () {}
    virtual ~alchemy () {}

    virtual void init () = 0;
    virtual void start () = 0;
    virtual void stop () = 0;

    virtual std::vector<std::unique_ptr<evaristo>> loop () = 0;

    const aristotele& info () const { return aristotele_; }

  protected:
    aristotele aristotele_;
    std::chrono::time_point<std::chrono::system_clock> start_time_; 
};
#endif
