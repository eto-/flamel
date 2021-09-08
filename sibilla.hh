// Sibilla manages the user options by command line or configuration file
// Sibilla can not be created but only evoked.
// Sibilla inherits from boost::program_options to simply the access to
// options.
#ifndef SIBILLA_HH
#define SIBILLA_HH
#include <boost/program_options.hpp>
#include <memory>

namespace po = boost::program_options;

class sibilla: public po::variables_map {
  public:
    static sibilla& evoke ();

    void parse (int argc, char *argv[]);
    bool operator() (const std::string& opt) { return count(opt); }

  private:
    po::options_description desc_;

    sibilla ();
    static std::unique_ptr<sibilla> me_;
};

#endif
