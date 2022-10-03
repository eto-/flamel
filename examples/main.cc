#include "wav_file.h"


int main(int, char *argv[]) {
  wav_file f(argv[1]);

  f.next();
  f.next();
  f.next();
  f.next();

  return 0;
}
