#include <fstream>
#include <pstreams/pstream.h>
#include "omero.hh"
#include "sibilla.hh"
#include "evaristo.hh"

omero::omero () {
  o_ = 0;
  if (sibilla::get ()("pretend")) return;

  filename_ = sibilla::get ()["filename"].as<std::string>();

  if (sibilla::get ()("zip")) {
    o_ = 0;
  } else o_ = std::unique_ptr<std::ostream> (new std::ofstream (filename_));
}

omero::~omero () {
//  o_->close ();
}

void omero::write (evaristo* ev) {
  *o_ << *ev;
}

