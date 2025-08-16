#include "./nox.hpp"
#include <cstdlib>

int main(int argc, char** argv) {
  int width = atoi(argv[2]);
  int height = atoi(argv[3]);

  Nox nox(argv[1], width, height);
  nox.videoToAscii();

  return 0;
}