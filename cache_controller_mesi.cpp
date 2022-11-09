#include "cache_controller_mesi.h"

CacheControllerMesi::CacheControllerMesi(Cache& cache, Bus& bus,
                                         SharedLine& shared_line)
: CacheController(cache, bus, shared_line)
{}

CacheControllerMesi::~CacheControllerMesi() = default;

void CacheControllerMesi::handle_core_op(CoreOp op) {

}

void CacheControllerMesi::handle_bus_transc(BusTransaction transc) {
  
}
