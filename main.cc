#include <iostream>
#include <stdexcept>
#include "sibilla.hh"
#include "giotto.hh"
#include "claudio.hh"
#include "omero.hh"

int main (int argc, char* argv[]) {
  sibilla::get ().parse (argc, argv);

  giotto g;
  claudio c;
  omero o;

  c.init ();
  c.start ();

  for (int n = sibilla::get ()["events"].as<int>(); n >= 0;) {
    auto v = c.loop ();
    n -= v.size ();
    for (int i = 0; i < v.size (); i++) {
      std::unique_ptr<evaristo> e = std::move(v[i]);
      if (!(n % 10)) g.draw (e.get ());
      o.write (e.get ());
    }
  }

  c.stop ();
}
