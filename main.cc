#include <iostream>
#include "claudio.hh"

int main () {
  claudio c;
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
}
