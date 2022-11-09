#include "core.h"
#include "bus.h"
#include "shared_line.h"

class CacheController {
public:
  virtual ~CacheController();
  virtual void handle_core_op(CoreOp op) = 0;
  virtual void handle_bus_resp(BusTransaction transc) = 0;
  virtual void handle_bus_transc(BusTransaction transc) = 0;

protected:
  CacheController(Cache& cache, Bus& bus, SharedLine& shared_line, size_t core_idx);

  Cache& cache;
  Bus& bus;
  SharedLine& shared_line;
  size_t core_idx;
};
