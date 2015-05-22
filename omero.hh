#ifndef OMERO_HH
#define OMERO_HH
#include <memory>

class evaristo;

class omero {
  public:
    omero ();
    ~omero ();

    void write (evaristo* ev);

  private:
};

#endif
