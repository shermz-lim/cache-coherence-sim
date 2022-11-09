#include "core.h"
#include "bus.h"
#include "shared_line.h"

class CacheController {
public:
  virtual void handle_core_op(CoreOp op) = 0;
  virtual void handle_bus_transc(BusTransaction transc) = 0;
  virtual ~CacheController() = 0;

protected:
  CacheController(Cache& cache, Bus& bus, SharedLine& shared_line);

  Cache& cache;
  Bus& bus;
  SharedLine& shared_line;
};
