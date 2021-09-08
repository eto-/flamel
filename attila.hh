// Attila manages logging and errors
// two macros are defined to add filename and linenumber:
// BRENNO simply dump on cerr the message
// ATTILA logs the message and trows a runtime_error exception
#ifndef ATTILA_HH
#define ATTILA_HH
#include <sstream>

class attila {
  public:
    attila(const std::string& file, int line = 0, bool fail=true);
#define ATTILA attila(__FILE__, __LINE__, true)
#define BRENNO attila(__FILE__, __LINE__, false)

    ~attila () noexcept(false);
    
    void exception ();

    template <typename T> attila& operator<<(const T &x) { s_ << x; return *this; }

    typedef std::basic_ostream<char, std::char_traits<char>>& (*manip)(std::basic_ostream<char, std::char_traits<char>>&);
    attila& operator<<(manip f) { f(s_); return *this; }
  private:
    std::ostringstream s_;
    bool fail_;
};
  
#endif
