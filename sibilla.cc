#include <iostream>
#include "sibilla.hh"

std::unique_ptr<sibilla> sibilla::me_(nullptr);

sibilla& sibilla::evoke () {
  if (!me_) me_ = std::unique_ptr<sibilla>(new sibilla);

  return *me_;
}

sibilla::sibilla (): desc_() {

  po::options_description link("link options");
  link.add_options()
    ("usb-link,U", "usb link type")
    ("link-number,L", po::value<int>()->default_value(0), "link number")
    ("node-number,N", po::value<int>()->default_value(0), "node number")
    ("vme-base,V", po::value<u_int32_t>()->default_value(0), "VME base address")
    ("emulate-hw,E", "emulate the digitizer")
    ;

  po::options_description file("file options");
  file.add_options()
    ("filename,f", po::value<std::string>(), "the output file name")
    ("zip,z", "zip output file")
    ("wav,w", "wav output format")
    ("txt,t", "txt output format")
    ("comment,c", po::value<std::string>(), "comment")
    ("run,r", po::value<int>()->default_value(0), "run number")
    ;

  po::options_description trg("trigger options");
  trg.add_options()
    ("nim,n", "use NIM trigger-in level (default TTL)")
    ("software-trigger,S", "enable software trigger")
    ("events,e", po::value<int>()->default_value(100), "events to acquire")
    ("gate-width,g", po::value<int>()->default_value(1000), "gate width in samples")
    ("post-trigger,p", po::value<int>()->default_value(50), "post trigger window (0-100)")
    ("overlap-trigger,O", "allow triggers to overlap")
    ("majority,m", po::value<int>()->default_value(1), "set the trigger majority (1...#channels)")
    ;

  po::options_description ch("channels options");
  ch.add_options()
    ("des-mode,d", "des mode")
    ("dc-offset,D", po::value<int>()->default_value(0xefff, "0xefff"), "channel dc offset (0x1000 for positive pulses and 0xefff for negative pulses)") 
    ("channel-threshold,T", po::value<int>()->default_value(-1), "channel threshold for auto-trigger, negative means disabled (value in samples)")
    ("positive-pulse,K", "positive pulse polarity (default is negative)") 
    ("channel-id,C", po::value<std::vector<int>>()->default_value(std::vector<int>{1}, "{1}"), "enabled channel (zero based, only odd if des-mode is enabled)")
    ("test-pattern,J", "enable test pattern")
    ;

  po::options_description other("other options");
  other.add_options()
    ("help,h", "produce help message")
    ("quiet,q", "do not run the display")
    ("prescale,R", po::value<int>()->default_value(10), "prescale the display");
    ;

  desc_.add(link).add(file).add(trg).add(ch).add(other);
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
