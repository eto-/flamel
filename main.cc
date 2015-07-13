#include <iostream>
#include <stdexcept>
#include <csignal>
#include "sibilla.hh"
#include "giotto.hh"
#include "flamel.hh"
#include "omero.hh"
#include <TSystem.h>

bool quit = false;
int main (int argc, char* argv[]) {
  gSystem->ResetSignal(kSigSegmentationViolation);
  gSystem->ResetSignal(kSigIllegalInstruction);
  gSystem->ResetSignal(kSigSystem);
  gSystem->ResetSignal(kSigPipe);
  gSystem->ResetSignal(kSigFloatingException);

  sibilla::evoke ().parse (argc, argv);

  giotto g;
  flamel f;
  f.init ();

  omero o(f.info ());

  signal(SIGINT, [](int signum) { std::cerr << "signal caught, clean exiting" << std::endl; quit = true; });
  sigset_t mask, orig_mask;
  sigemptyset (&mask);
  sigaddset (&mask, SIGINT);

  sleep(3);
  f.start ();
  int c = 0;
  int prescale = sibilla::evoke ()["prescale"].as<int>();

  for (int n = sibilla::evoke ()["events"].as<int>(); n > 0;) {
    if (quit) break;
    
    sigprocmask(SIG_BLOCK, &mask, &orig_mask);

    auto v = f.loop ();

    sigprocmask(SIG_SETMASK, &orig_mask, NULL);

    n -= v.size ();
    for (int i = 0; i < v.size (); i++) {
      std::unique_ptr<evaristo> e = std::move(v[i]);
      if (!(c++ % prescale)) g.draw (e.get ());
      o.write (e.get ());
    }
  }

  f.stop ();
}
