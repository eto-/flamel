// paracelsus is the base daq class
//   - init initialize the digitizer
//   - star & stop manage the run
//   - loop fetches new data from the digitizer
//   - info returns metadata
#ifndef PARACELSUS_HH
#define PARACELSUS_HH

#include <vector>
#include <memory>
#include <chrono>
#include "evaristo.hh"

class paracelsus {
  public:
    paracelsus () {}
    virtual ~paracelsus () {}

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
