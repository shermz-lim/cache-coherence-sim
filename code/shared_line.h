#pragma once

#include <vector>

#include "cache.h"

class SharedLine {
public:
  SharedLine(std::vector<Cache>& caches);
  // returns whether block with block_no is present in any cache other than mine
  bool assert_line(CacheBlock block_no, size_t my_core);

private:
  std::vector<Cache>& caches;
};
