#include <iostream>

const size_t DEF_CACHE_SIZE = 4096;
const size_t DEF_ASSOC = 2;
const size_t DEF_BLOCK_SIZE = 32;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    throw std::runtime_error{"too few arguments..."};
  }
  std::string protocol{argv[1]};
  std::string input_file{argv[2]};
  size_t cache_size = DEF_CACHE_SIZE;
  size_t assoc = DEF_ASSOC;
  size_t block_size = DEF_BLOCK_SIZE;
  if (argc >= 4) {
    sscanf(argv[3], "%zu", &cache_size);
  }
  if (argc >= 5) {
    sscanf(argv[4], "%zu", &assoc);
  }
  if (argc >= 6) {
    sscanf(argv[5], "%zu", &block_size);
  }
}
