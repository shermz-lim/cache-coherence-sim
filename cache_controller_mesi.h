#pragma once

#include <map>
#include <string_view>

#include "cache_controller.h"

class CacheControllerMesi : public CacheController {
public:
  CacheControllerMesi(size_t core_no, Cache& cache, Bus& bus, SharedLine& shared_line);
  ~CacheControllerMesi() override;

  bool handle_core_op(CoreOp op) override;
  void handle_bus_resp(BusTransaction transc) override;
  bool handle_bus_transc(BusTransaction transc) override;
  void evict_block(CacheBlock block_no) override;

  void print_state() override;

private:
  enum class State {
    INVALID,
    EXCLUSIVE,
    SHARED,
    MODIFIED
  };

  static std::string_view state_to_string(State s);

  State get_state(CacheBlock block_no);
  void update_state(CacheBlock block_no, State state);

  std::map<CacheBlock, State> blocks_state{};
};
