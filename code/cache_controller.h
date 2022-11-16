#pragma once

#include "core.h"
#include "bus.h"
#include "shared_line.h"

struct CacheControllerStats {
  size_t misses{0};
  size_t tot_access{0};
  size_t priv_access{0};
  size_t shared_access{0};
};

class CacheController {
public:
  virtual ~CacheController();
  // handles a core's read/write. May update cache/block states/add transaction request to bus
  // returns whether it's completed successfully (cache hit in 1 cycle)
  virtual bool handle_core_op(CoreOp op) = 0;
  // handles response to previous transaction placed on bus in handle_core_op
  virtual void handle_bus_resp(BusTransaction transc) = 0;
  // handles snooped bus transaction from other cores. Returns whether flush to memory is required
  virtual bool handle_bus_transc(BusTransaction transc) = 0;
  // evict block
  virtual void evict_block(CacheBlock block_no) = 0;

  virtual void print_state() = 0;

  inline CacheControllerStats get_stats() {
    return stats;
  }

protected:
  CacheController(size_t core_no, Cache& cache, Bus& bus, SharedLine& shared_line);

  size_t core_no;
  Cache& cache;
  Bus& bus;
  SharedLine& shared_line;
  CacheControllerStats stats;
};
