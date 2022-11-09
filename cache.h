#include <vector>
#include <list>
#include <unordered_map>
#include <optional>

#include "cache_types.h"

class Cache {
public:
  Cache(size_t size, size_t assoc, size_t block_size);

  inline CacheBlock get_block_no(size_t mem_addr) {
    return mem_addr / block_size;
  }

  bool has_block(CacheBlock block_no);
  // attempt to read block and returns whether there's a cache hit
  bool read_block(CacheBlock block_no);
  // attempt to write block and returns whether there's a cache hit
  bool write_block(CacheBlock block_no);
  // insert block, and assumes there's space
  void insert_block(CacheBlock block_no);
  // maybe evict a block if there's no space to insert CacheBlock with block_no
  // returns evicted cache block if any, and whether it's dirty
  std::optional<std::pair<CacheBlock, bool>> maybe_evict_block(CacheBlock block_no);
  void print_state();

private:
  struct CacheSet {
    // map of cache block to dirty bit
    std::unordered_map<CacheBlock, bool> blocks{};
    // front is most recently used
    std::list<CacheBlock> blocks_access_order{};
  };

  inline size_t get_set_index(CacheBlock block_no) {
    return block_no % cache_sets.size();
  }

  inline CacheSet& get_cache_set(CacheBlock block_no) {
    return cache_sets.at(get_set_index(block_no));
  }

  bool access_block(CacheBlock block_no, bool modify);

  size_t assoc;
  size_t block_size;
  std::vector<CacheSet> cache_sets;
};
