#pragma once

#include <unordered_map>
#include <string_view>

#include "cache_controller.h"

class CacheControllerDragon : public CacheController {
public:
  CacheControllerDragon(size_t core_no, Cache& cache, Bus& bus, SharedLine& shared_line);
  ~CacheControllerDragon() override;

  bool handle_core_op(CoreOp op) override;
  void handle_bus_resp(BusTransaction transc) override;
  bool handle_bus_transc(BusTransaction transc) override;
  void evict_block(CacheBlock block_no) override;

  void print_state() override;

private:
  enum class State {
    INVALID,
    EXCLUSIVE,
    SHARED_CLEAN,
    SHARED_MODIFIED,
    MODIFIED
  };

  static std::string_view state_to_string(State s);

  State get_state(CacheBlock block_no);
  void update_state(CacheBlock block_no, State state);

  std::unordered_map<CacheBlock, State> blocks_state{};
};
