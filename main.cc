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
  int max_events = sibilla::evoke ()["events"].as<int>();
  int max_duration = sibilla::evoke ()["duration"].as<int>();

  int event_count = 0;
  auto start_t = std::chrono::system_clock::now ();
  float run_t = 0;

  int prev_n, progress_dt = sibilla::evoke ()["progress"].as<int>();
  auto next_t = start_t + std::chrono::duration_cast<std::chrono::seconds>(std::chrono::seconds(progress_dt));
  int prescale_plot = sibilla::evoke ()["prescale"].as<int>();

  while (1) {
    run_t = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now () - start_t).count();

    if (quit) break;
    if (max_events > 0 && event_count >= max_events) break;
    if (max_duration > 0 && run_t >= max_duration) break;

    if (progress_dt > 0 && std::chrono::system_clock::now () > next_t) {
      std::cerr << "Processed " << event_count << " events in " << run_t << " s at rate of " << event_count / run_t << " (" << float(event_count - prev_n) / progress_dt << ") cps" << std::endl;
      next_t += std::chrono::duration_cast<std::chrono::seconds>(std::chrono::seconds(progress_dt));
      prev_n = event_count;
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

    event_count += v.size ();
    for (size_t i = 0; i < v.size (); i++) {
      std::unique_ptr<evaristo> e = std::move(v[i]);
      if (!(event_count++ % prescale_plot)) g.draw (e.get ());
      o.write (e.get ());
    }
  }

  std::cout << "Acquired " << event_count << " events in " << run_t << " s at rate of " << event_count / run_t << " cps" << std::endl;

  a->stop ();
}
