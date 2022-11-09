#include "cache_controller.h"

class CacheControllerMesi : public CacheController {
public:
  CacheControllerMesi(Cache& cache, Bus& bus, SharedLine& shared_line);
  ~CacheControllerMesi() override;

  void handle_core_op(CoreOp op) override;
  void handle_bus_transc(BusTransaction transc) override;

private:

};
