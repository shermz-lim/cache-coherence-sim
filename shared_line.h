#include <vector>

#include "cache.h"

class SharedLine {
public:
  SharedLine(std::vector<Cache>& caches);
  // returns whether block with block_no is present in any cache
  bool assert_line(CacheBlock block_no);

private:
  std::vector<Cache>& caches;
};
