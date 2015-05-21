#ifndef ATTILA_HH
#define ATTILA_HH
#include <sstream>

class attila {
  public:
    attila(const std::string& file, bool fail=true);
    ~attila ();
    
    void exception ();

    template <class T> attila& operator<<(const T &x) { s_ << x; return *this; }
  private:
    std::ostringstream s_;
    bool fail_;
};
  
#endif
