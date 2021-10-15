#include <iostream>
#include <stdexcept>
#include <csignal>
#include "sibilla.hh"
#include "giotto.hh"
#include "flamel.hh"
#include "stgermain.hh"
#include "omero.hh"
#include <TSystem.h>
#include <chrono>

bool quit = false;
int main (int argc, char* argv[]) { 
  gSystem->ResetSignal(kSigSegmentationViolation);
  gSystem->ResetSignal(kSigIllegalInstruction);
  gSystem->ResetSignal(kSigSystem);
  gSystem->ResetSignal(kSigPipe);
  gSystem->ResetSignal(kSigFloatingException);

  sibilla::evoke ().parse (argc, argv);

  giotto g;
  alchemy *a;
  if (sibilla::evoke ()["host"].as<std::string> ().empty ()) a = new flamel;
  else a = new stgermain;

  a->init ();

  omero o(a->info ());

  signal(SIGINT, [](int signum) { std::cerr << "signal caught, clean exiting" << std::endl; quit = true; });
  sigset_t mask, orig_mask;
  sigemptyset (&mask);
  sigaddset (&mask, SIGINT);

  sleep(3);
  a->start ();
  int c = 0;
  int prescale = sibilla::evoke ()["prescale"].as<int>();

  std::chrono::seconds seconds(sibilla::evoke ()["progress"].as<int>());
  auto start = std::chrono::system_clock::now ();
  auto next = start + std::chrono::duration_cast<std::chrono::seconds>(seconds);

  int total = sibilla::evoke ()["events"].as<int>();
  int n = 0;
  while (n < total) {
    if (quit) break;

    if (seconds.count () > 0 && std::chrono::system_clock::now () > next) {
      float dt = std::chrono::duration_cast<std::chrono::seconds>(next - start).count();
      std::cerr << "Processed " << n << " events in " << dt << " s at rate of " << n / dt << " cps" << std::endl;
      next += std::chrono::duration_cast<std::chrono::seconds>(seconds);
    }
    
    sigprocmask(SIG_BLOCK, &mask, &orig_mask);

    std::vector<std::unique_ptr<evaristo>> v;

    try {
      v = a->loop ();
    } catch (std::runtime_error &e) {
      std::cerr << "Exception in flamel loop: " << e.what() << std::endl;
      std::cerr << "Clean exiting" << std::endl;
      break;
    }

    sigprocmask(SIG_SETMASK, &orig_mask, NULL);

    n += v.size ();
    for (size_t i = 0; i < v.size (); i++) {
      std::unique_ptr<evaristo> e = std::move(v[i]);
      if (!(c++ % prescale)) g.draw (e.get ());
      o.write (e.get ());
    }
  }
  float dt = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now () - start).count();
  std::cout << "Acquired " << n << " events in " << dt << " s at rate of " << n / dt << " cps" << std::endl;

  a->stop ();
}
