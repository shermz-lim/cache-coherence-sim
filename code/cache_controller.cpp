#include "cache_controller.h"

CacheController::~CacheController() = default;

CacheController::CacheController(size_t core_no, Cache& cache, Bus& bus,
                                 SharedLine& shared_line)
: core_no(core_no), cache(cache), bus(bus),
  shared_line(shared_line)
{}
