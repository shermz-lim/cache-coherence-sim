#include "shared_line.h"

SharedLine::SharedLine(std::vector<Cache>& caches)
: caches(caches)
{}

bool SharedLine::assert_line(CacheBlock block_no) {
  for (Cache& cache : caches) {
    if (cache.has_block(block_no)) {
      return true;
    }
  }
  return false;
}
