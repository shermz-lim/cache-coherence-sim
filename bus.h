#pragma once

#include <queue>
#include <optional>

#include "core.h"
#include "cache_types.h"

enum class BusTransactionType {
  BUS_RD,
  BUS_RDX,
  BUS_WB,
  BUS_UPGR
};

struct BusTransaction {
  BusTransactionType t;
  CacheBlock block;
  // core op that triggered this transaction
  CoreOp op_trigger;

  auto operator<=>(const BusTransaction&) const = default;
};

class Bus {
public:
  inline bool has_request() {
    return !requests.empty();
  }

  inline void add_request(BusTransaction transc) {
    requests.push(transc);
  }

  inline bool has_curr_transc() {
    return curr_transc.has_value();
  }

  inline void curr_transc_complete() {
    curr_transc = std::nullopt;
  }

  BusTransaction next_transc();

private:
  std::queue<BusTransaction> requests;
  std::optional<BusTransaction> curr_transc;
};
