#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

const size_t DEF_CACHE_SIZE = 4096;
const size_t DEF_ASSOC = 2;
const size_t DEF_BLOCK_SIZE = 32;
const std::string DATA_DIR{"data"};

std::vector<std::pair<int, size_t>> parse_core_input(
  std::string input_file,
  size_t core_no
) {
  std::ostringstream path;
  path << DATA_DIR << "/" << input_file << "_" << core_no << ".data";
  std::ifstream f(path.str());
  
  std::vector<std::pair<int, size_t>> raw_ops;
  int label;
  size_t value;
  while (f >> std::dec >> label >> std::hex >> value) {
    std::cout << label << " " << value << std::endl;
    raw_ops.push_back(std::make_pair(label, value));
  }
  return raw_ops;
}

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
