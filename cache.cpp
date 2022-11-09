#include <iostream>
#include "cache.h"

Cache::Cache(size_t size, size_t assoc, size_t block_size)
: assoc(assoc),
  block_size(block_size),
  cache_sets(size / block_size / assoc)
{
}

bool Cache::read_block(CacheBlock block_no) {
  return access_block(block_no, false);
}

bool Cache::write_block(CacheBlock block_no) {
  return access_block(block_no, true);
}

void Cache::insert_block(CacheBlock block_no) {
  CacheSet& cache_set = get_cache_set(block_no);
  if (cache_set.blocks.size() >= assoc) {
    throw std::runtime_error("inserting block into full cache set");
  }

  cache_set.blocks.insert(std::make_pair(block_no, false));
  cache_set.blocks_access_order.push_front(block_no);
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
