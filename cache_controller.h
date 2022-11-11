#include "core.h"
#include "bus.h"
#include "shared_line.h"

class CacheController {
public:
  virtual ~CacheController();
  // handles a core's read/write. May update cache/block states/add transaction request to bus
  virtual void handle_core_op(CoreOp op) = 0;
  // handles response to previous transaction placed on bus in handle_core_op
  virtual void handle_bus_resp(BusTransaction transc) = 0;
  // handles snooped bus transaction from other cores. Returns whether flush to memory is required
  virtual bool handle_bus_transc(BusTransaction transc) = 0;

protected:
  CacheController(Cache& cache, Bus& bus, SharedLine& shared_line, size_t core_idx);

  Cache& cache;
  Bus& bus;
  SharedLine& shared_line;
  size_t core_idx;
};
