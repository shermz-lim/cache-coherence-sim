#pragma once

#include "cache_controller_mesi.h"

class CacheControllerIMesi : public CacheControllerMesi {
public:
  CacheControllerIMesi(size_t core_no, Cache& cache, Bus& bus, SharedLine& shared_line);
  BusTranscOutput handle_bus_transc(BusTransaction transc) override;
};
