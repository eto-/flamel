#include <iostream>
#include <stdexcept>
#include "sibilla.hh"
#include "giotto.hh"
#include "flamel.hh"
#include "omero.hh"

int main (int argc, char* argv[]) {
  sibilla::evoke ().parse (argc, argv);

  giotto g;
  flamel f;
  omero o;

  std::string metadata = f.init ();
  o.metadata (metadata);

  f.start ();

  for (int n = sibilla::evoke ()["events"].as<int>(); n > 0;) {
    auto v = f.loop ();
    n -= v.size ();
    for (int i = 0; i < v.size (); i++) {
      std::unique_ptr<evaristo> e = std::move(v[i]);
      if (!(n % 5)) g.draw (e.get ());
      o.write (e.get ());
    }
  }

  f.stop ();
}
