#include <iostream>
#include "claudio.hh"

int main () {
  claudio c;
  for (int i = 0; i < 20; i++) {
    std::cout << "calling loop" << std::endl;
    c.loop ();
  }
}
