#include <iostream>
#include <stdexcept>
#include "attila.hh"

attila::attila (const std::string& file, bool fail): fail_(fail) {
  if (file.size ()) s_ << file << ": ";
}

attila::~attila () {
  exception ();
}

void attila::exception () {
  if (!s_.str ().size ()) s_ << "-";
  std::cerr << "!!! " << s_.str ();
  if (*(s_.str ().rbegin ()) != '\n') std::cerr << "\n";

  if (fail_) throw std::runtime_error(s_.str());
}
