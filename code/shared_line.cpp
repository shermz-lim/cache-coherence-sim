#include "shared_line.h"

SharedLine::SharedLine(std::vector<Cache>& caches)
: caches(caches)
{}

bool SharedLine::assert_line(CacheBlock block_no, size_t my_core) {
  for (size_t core_no = 0; core_no < caches.size(); core_no++) {
    if (core_no == my_core) continue;
    if (caches.at(core_no).has_block(block_no)) {
      return true;
    }
  }
  return false;
}
