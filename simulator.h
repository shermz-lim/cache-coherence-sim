#pragma once

#include <set>

#include "cache_controller.h"

struct BusRequest {
  BusTransaction transc;
  CoreOp op_trigger;
};

struct BusResponse {
  BusTransaction transc;
  CoreOp op_trigger;
};

using Event = std::variant<BusRequest, BusResponse, CoreOp>;
using EventQueue = std::set<std::pair<size_t, Event>>;

class Simulator {
public:
  Simulator(std::vector<Core>& cores, std::vector<Cache>& caches,
            Bus& bus, SharedLine& shared_line,
            std::vector<std::unique_ptr<CacheController>>& cache_controllers);

  void simulate();

private:
  std::vector<Core>& cores;
  std::vector<Cache>& caches;
  Bus& bus;
  SharedLine& shared_line;
  std::vector<std::unique_ptr<CacheController>>& cache_controllers;
};
