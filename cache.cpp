#include <assert.h>
#include <iostream>

#include "cache.h"

Cache::Cache(size_t size, size_t assoc, size_t block_size)
: assoc(assoc),
  block_size(block_size),
  cache_sets(size / block_size / assoc)
{
}

bool Cache::has_block(CacheBlock block_no) {
  return get_cache_set(block_no).blocks.count(block_no);
}

bool Cache::read_block(CacheBlock block_no) {
  return access_block(block_no, false);
}

bool Cache::write_block(CacheBlock block_no) {
  return access_block(block_no, true);
}

void Cache::insert_block(CacheBlock block_no) {
  CacheSet& cache_set = get_cache_set(block_no);
  assert(cache_set.blocks.size() < assoc);
  assert(!has_block(block_no));

  cache_set.blocks.insert(std::make_pair(block_no, false));
  cache_set.blocks_access_order.push_front(block_no);
}

std::optional<std::pair<CacheBlock, bool>> Cache::maybe_evict_block(CacheBlock block_no) {
  CacheSet& cache_set = get_cache_set(block_no);
  if (cache_set.blocks.size() < assoc) {
    return std::nullopt;
  } else {
    CacheBlock to_evict = cache_set.blocks_access_order.back();
    bool dirty_bit = cache_set.blocks.at(to_evict);
    cache_set.blocks_access_order.remove(to_evict);
    cache_set.blocks.erase(to_evict);
    return std::make_pair(to_evict, dirty_bit);
  }
}

void Cache::print_state() {
  std::cout << "------- Cache state -------\n";
  for (size_t i = 0; i < cache_sets.size(); i++) {
    std::cout << "Cache set " << i << ":\n";
    const CacheSet& cache_set = cache_sets.at(i);
    for (CacheBlock no : cache_set.blocks_access_order) {
      std::cout << no << ":" << cache_set.blocks.at(no) << "; ";
    }
    std::cout << std::endl;
  }
}

bool Cache::access_block(CacheBlock block_no, bool modify) {
  CacheSet& cache_set = get_cache_set(block_no);
  if (cache_set.blocks.count(block_no)) {
    cache_set.blocks_access_order.remove(block_no);
    cache_set.blocks_access_order.push_front(block_no);
    if (modify) {
      cache_set.blocks.at(block_no) = true;
    }
    return true;
  } else {
    return false;
  }
}
