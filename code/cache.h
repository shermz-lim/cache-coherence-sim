#pragma once

#include <vector>
#include <list>
#include <unordered_map>
#include <optional>

#include "cache_types.h"

class Cache {
public:
  Cache(size_t core_no, size_t size, size_t assoc, size_t block_size);

  inline CacheBlock get_block_no(size_t mem_addr) {
    return mem_addr / block_size;
  }

  bool has_block(CacheBlock block_no);
  // read block, assuming it's there
  void read_block(CacheBlock block_no);
  // write block, assuming it's there
  void write_block(CacheBlock block_no);
  // insert block, and assumes there's space
  void insert_block(CacheBlock block_no);
  // remove block, and returns whether it's dirty
  bool remove_block(CacheBlock block_no);
  // void clear dirty bit of block, assumes it's there
  void clear_dirty_bit(CacheBlock block_no);
  // returns block to evict if there's no space to insert CacheBlock with block_no
  // returns block and whether it's dirty
  std::optional<std::pair<CacheBlock, bool>> block_to_evict(CacheBlock block_no);
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

  void access_block(CacheBlock block_no, bool modify);

  size_t core_no;
  size_t assoc;
  size_t block_size;
  std::vector<CacheSet> cache_sets;
};
