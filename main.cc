#include <iostream>
#include <stdexcept>
#include "claudio.hh"
#include "sibilla.hh"

int main (int argc, char* argv[]) {
  sibilla::get ().parse (argc, argv);

  claudio c;
  c.init ();
  c.start ();

  for (int n = sibilla::get ()["events"].as<int>(); n >= 0;) {
    auto v = c.loop ();
    n -= v.size ();
    for (int i = 0; i < v.size (); i++) {
      std::unique_ptr<evaristo> e = std::move(v[i]);
      std::cout << e->time_tag << std::endl; 
    }
  }

  c.stop ();
}
