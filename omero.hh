#ifndef OMERO_HH
#define OMERO_HH
#include <string>
#include <memory>
#include <ostream>

class evaristo;

class omero {
  public:
    omero ();

    void write (evaristo* ev);

  private:
    std::string filename_;
    std::unique_ptr<std::ostream> o_;
};

#endif
