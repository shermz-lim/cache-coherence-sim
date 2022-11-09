#include "cache_controller.h"

CacheController::~CacheController() = default;

CacheController::CacheController(Cache& cache, Bus& bus,
                                 SharedLine& shared_line,
                                 size_t core_idx)
: cache(cache), bus(bus),
  shared_line(shared_line),
  core_idx(core_idx)
{}
