#include <unordered_map>

#include "cache_controller.h"

class CacheControllerMesi : public CacheController {
public:
  CacheControllerMesi(Cache& cache, Bus& bus, SharedLine& shared_line,
                      size_t core_idx);
  ~CacheControllerMesi() override;

  void handle_core_op(CoreOp op) override;
  void handle_bus_resp(BusTransaction transc) override;
  void handle_bus_transc(BusTransaction transc) override;

private:
  enum class State {
    INVALID,
    EXCLUSIVE,
    SHARED,
    MODIFIED
  };

  State get_state(CacheBlock block_no);
  void update_state(CacheBlock block_no, State state);

  std::unordered_map<CacheBlock, State> blocks_state{};
};
