#ifndef OMERO_HH
#define OMERO_HH
#include <string>
#include <memory>
#include <ostream>
#include <sndfile.hh>


class evaristo;

class omero {
  public:
    omero ();

    void metadata (const std::string&);
    void write (evaristo*);

  private:
    std::string filename_;
    std::unique_ptr<std::ostream> o_txt_;
    std::unique_ptr<SndfileHandle> o_wav_;
    
};

#endif
