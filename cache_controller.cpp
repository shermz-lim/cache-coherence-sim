#include "cache_controller.h"

CacheController::~CacheController() = default;

CacheController::CacheController(Cache& cache, Bus& bus,
                                 SharedLine& shared_line)
: cache(cache), bus(bus), shared_line(shared_line)
{}
