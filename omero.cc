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
    if (filename_.size () < 3 || filename_.substr(filename_.size () - 3) != ".gz") filename_ = filename_ + ".gz";

    o_ = std::unique_ptr<std::ostream> (new redi::opstream ("gzip - > " + filename_));
  } else o_ = std::unique_ptr<std::ostream> (new std::ofstream (filename_));

  *o_ << "run: " << sibilla::get ()["run"].as<int>() << std::endl;
  if (sibilla::get ()("comment")) *o_ << "comment: " << sibilla::get ()["comment"].as<std::string>() << std::endl;
}

void omero::write (evaristo* ev) {
  *o_ << *ev;
}

