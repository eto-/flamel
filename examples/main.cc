#include "wav_file.h"


int main(int, char *argv[]) {
  wav_file f(argv[1]);

  for (int i = 0; i < 10 && !f.eof(); ++i) {
    auto e = f.next();
    std::cout << e.counter << " " << e.data[0][0] << std::endl; 
  }

  return 0;
}
