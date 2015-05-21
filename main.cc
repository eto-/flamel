#include <iostream>
#include <stdexcept>
#include "claudio.hh"
#include "sibilla.hh"

int main (int argc, char* argv[]) {
  sibilla::get ().parse (argc, argv);

  claudio c;
  c.init ();
  c.start ();

  for (int i = 0; i < 20; i++) {
    auto v = c.loop ();
    std::cout << "calling loop " << v.size () << std::endl;
    while (v.size ()) {
      claudio_ev *e = v.front ();
      std::cout << e->time_tag << std::endl; 
      delete [] e; 
      v.erase (v.begin ());
    }
  }

  c.stop ();
}
