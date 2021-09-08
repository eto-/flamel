// Omero writes the event on disk using a wav format
// (or aleternately in plain text)
#ifndef OMERO_HH
#define OMERO_HH
#include <string>
#include <memory>
#include <ostream>
#include <sndfile.hh>


class evaristo;
class aristotele;

class omero {
  public:
    omero (const aristotele&);

    void write (evaristo*);

  private:
    std::string filename_;
    std::unique_ptr<std::ostream> o_txt_;
    std::unique_ptr<SndfileHandle> o_wav_;
    
};

#endif
