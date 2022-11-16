#pragma once

#include <list>
#include <optional>

#include "core.h"
#include "cache_types.h"

enum class BusTransactionType {
  BUS_RD,
  BUS_RDX,
  BUS_WB
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
  inline bool has_request() {
    return !requests.empty();
  }

  inline void add_request(BusTransaction transc) {
    requests.push_back(transc);
  }

  inline bool has_curr_transc() {
    return curr_transc.has_value();
  }

  inline BusStats get_stats() {
    return stats;
  }

  void curr_transc_complete();

  BusTransaction next_transc();

  void print_state();

private:
  std::list<BusTransaction> requests;
  std::optional<BusTransaction> curr_transc;
  BusStats stats;
};
