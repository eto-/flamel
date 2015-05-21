#include <iostream>
#include "sibilla.hh"

std::unique_ptr<sibilla> sibilla::me_(nullptr);

sibilla& sibilla::get () {
  if (!me_) me_ = std::unique_ptr<sibilla>(new sibilla);

  return *me_;
}

sibilla::sibilla (): desc_("claudio options") {
  desc_.add_options()
    ("help,h", "produce help message")
    ("usb-link,u", "usb link type")
    ("link-number,l", po::value<int>()->default_value(0), "link number")
    ("pretend,P", "skip file saving")
    ("zip,z", "zip output file")
    ("run,r", po::value<int>()->required(), "run number")
    ("events,e", po::value<int>()->required(), "events to acquire")
    ("gate-width,g", po::value<int>()->required(), "gate width in samples")
    ("post-trigger,p", po::value<int>()->default_value(0), "post trigger window (0-100)")
    ;
}

void sibilla::parse (int argc, char *argv[]) {
  try {
    po::store(po::parse_command_line(argc, argv, desc_), *this);
    po::notify(*this);
  } catch (std::exception& e) {
    if (!count("help")) {
      std::cerr << e.what () << std::endl;
      std::cerr << desc_ << std::endl;
      exit(1);
    }
  }

  if (count("help")) {
    std::cerr << desc_ << std::endl;
    exit (0);
  }
}
