#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>

#include "simulator.h"
#include "cache_controller_mesi.h"
#include "cache_controller_dragon.h"

const size_t DEF_CACHE_SIZE = 4096;
const size_t DEF_ASSOC = 2;
const size_t DEF_BLOCK_SIZE = 32;
const std::string DATA_DIR{"data"};
const size_t NUM_CORES = 4;

const std::string MESI_PROTOCOL_NAME{"MESI"};
const std::string DRAGON_PROTOCOL_NAME{"Dragon"};

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
  
  // create components
  std::vector<Core> cores{};
  std::vector<Cache> caches{};
  for (size_t core_no = 0; core_no < NUM_CORES; core_no++) {
    auto core_raw_ops = parse_core_input(
      input_file,
      core_no
    );
    cores.emplace_back(core_no, core_raw_ops);
    caches.emplace_back(core_no, cache_size, assoc, block_size);
  }
  Bus bus{};
  SharedLine shared_line{caches};
  std::vector<std::unique_ptr<CacheController>> cache_controllers{};
  for (size_t core_no = 0; core_no < NUM_CORES; core_no++) {
    std::unique_ptr<CacheController> c{};
    if (protocol == MESI_PROTOCOL_NAME) {
      c = std::make_unique<CacheControllerMesi>(
        core_no, caches.at(core_no), bus, shared_line
      );
    } else if (protocol == DRAGON_PROTOCOL_NAME) {
      c = std::make_unique<CacheControllerDragon>(
        core_no, caches.at(core_no), bus, shared_line
      );
    } else {
      throw std::runtime_error{"invalid protocol name"};
    }
    cache_controllers.push_back(std::move(c));
  }

  Simulator s{block_size, cores, caches, bus, shared_line, cache_controllers};
  s.simulate();
}
