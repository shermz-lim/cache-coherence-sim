#include <assert.h>
#include <iostream>

#include "cache.h"

Cache::Cache(size_t core_no, size_t size, size_t assoc, size_t block_size)
: core_no(core_no),
  assoc(assoc),
  block_size(block_size),
  cache_sets(size / block_size / assoc)
{
}

bool Cache::has_block(CacheBlock block_no) {
  return get_cache_set(block_no).blocks.count(block_no);
}

void Cache::read_block(CacheBlock block_no) {
  return access_block(block_no, false);
}

void Cache::write_block(CacheBlock block_no) {
  return access_block(block_no, true);
}

void Cache::insert_block(CacheBlock block_no) {
  CacheSet& cache_set = get_cache_set(block_no);
  assert(cache_set.blocks.size() < assoc);
  assert(!has_block(block_no));

  cache_set.blocks.insert(std::make_pair(block_no, false));
  cache_set.blocks_access_order.push_front(block_no);
}

bool Cache::remove_block(CacheBlock block_no) {
  assert(has_block(block_no));

  CacheSet& cache_set = get_cache_set(block_no);
  bool dirty_bit = cache_set.blocks.at(block_no);
  cache_set.blocks_access_order.remove(block_no);
  cache_set.blocks.erase(block_no);
  return dirty_bit;
}

void Cache::clear_dirty_bit(CacheBlock block_no) {
  assert(has_block(block_no));
  CacheSet& cache_set = get_cache_set(block_no);
  assert(cache_set.blocks.at(block_no));
  cache_set.blocks.at(block_no) = false;
}

std::optional<std::pair<CacheBlock, bool>> Cache::block_to_evict(CacheBlock block_no) {
  CacheSet& cache_set = get_cache_set(block_no);
  if (cache_set.blocks.size() < assoc) {
    return std::nullopt;
  } else {
    CacheBlock to_evict = cache_set.blocks_access_order.back();
    bool dirty_bit = cache_set.blocks.at(to_evict);
    return std::make_pair(to_evict, dirty_bit);
  }
}

void Cache::print_state() {
  std::cout << "------- Cache " << core_no << " state -------\n";
  for (size_t i = 0; i < cache_sets.size(); i++) {
    std::cout << "set " << i << ": ";
    const CacheSet& cache_set = cache_sets.at(i);
    for (CacheBlock no : cache_set.blocks_access_order) {
      std::cout << no << ":" << cache_set.blocks.at(no) << "; ";
    }
    std::cout << std::endl;
  }
}

void Cache::access_block(CacheBlock block_no, bool modify) {
  assert(has_block(block_no));
  CacheSet& cache_set = get_cache_set(block_no);
  cache_set.blocks_access_order.remove(block_no);
  cache_set.blocks_access_order.push_front(block_no);
  if (modify) {
    cache_set.blocks.at(block_no) = true;
  }
}
