#include <fstream>
#include <pstreams/pstream.h>
#include <time.h>
#include "omero.hh"
#include "sibilla.hh"
#include "evaristo.hh"

omero::omero () {
  o_ = 0;
  if (sibilla::evoke ()("pretend")) return;

  filename_ = sibilla::evoke ()["filename"].as<std::string>();

  if (sibilla::evoke ()("zip")) {
    if (filename_.size () < 3 || filename_.substr(filename_.size () - 3) != ".gz") filename_ = filename_ + ".gz";

    o_ = std::unique_ptr<std::ostream> (new redi::opstream ("gzip - > " + filename_));
  } else o_ = std::unique_ptr<std::ostream> (new std::ofstream (filename_));

  *o_ << "run: " << sibilla::evoke ()["run"].as<int>() << std::endl;
  time_t t = time (NULL);
  *o_ << "date: " << ctime (&t);
  if (sibilla::evoke ()("comment")) *o_ << "comment: " << sibilla::evoke ()["comment"].as<std::string>() << std::endl;
}

void omero::metadata (const std::string& m) {
  if (!m.size ()) return;

  *o_ << m;
  if (m[m.size () - 1] != '\n') *o_ << std::endl;
}

void omero::write (evaristo* ev) {
  if (!ev) return;

  *o_ << *ev;
}

