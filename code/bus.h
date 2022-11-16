#pragma once

#include <list>
#include <unordered_map>
#include <optional>

#include "core.h"
#include "cache_types.h"

const size_t NUM_CONCURRENT_TRANSC = 4;

enum class BusTransactionType {
  BUS_RD,
  BUS_RDX,
  BUS_WB,
  BUS_UPD
};

struct BusTransaction {
  BusTransactionType t;
  CacheBlock block;
  // core op that triggered this transaction
  CoreOp op_trigger;

  auto operator<=>(const BusTransaction&) const = default;
  std::string to_string();

private:
  std::string_view type_to_string();
};

struct BusStats {
  size_t num_inv_upds{0};
  size_t blks_traffic{0};
};

class Bus {
public:
  inline void add_request(BusTransaction transc) {
    requests[transc.block].push_back(transc);
  }

  inline BusStats get_stats() {
    return stats;
  }

  void curr_transc_complete(BusTransaction transc);

  std::vector<BusTransaction> next_transcs();

  void print_state();

private:
  std::unordered_map<CacheBlock, std::list<BusTransaction>> requests;
  std::unordered_map<CacheBlock, BusTransaction> curr_transcs;
  BusStats stats;
};
