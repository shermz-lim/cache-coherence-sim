#include <vector>
#include <list>
#include <unordered_map>

// Cache blocks are identified by their block numbers
using CacheBlock = size_t;

class Cache {
public:
  Cache(size_t size, size_t assoc, size_t block_size);

  inline CacheBlock get_block_no(size_t mem_addr) {
    return mem_addr / block_size;
  }

  bool read_block(CacheBlock block_no);
  bool write_block(CacheBlock block_no);
  void insert_block(CacheBlock block_no);

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
