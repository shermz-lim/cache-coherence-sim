#pragma once

#include <set>

#include "cache_controller.h"

struct BusRequest {
  BusTransaction transc;

  auto operator<=>(const BusRequest&) const = default;
};

struct BusResponse {
  BusTransaction transc;

  auto operator<=>(const BusResponse&) const = default;
};

struct CoreOpStart {
  CoreOp op;

  auto operator<=>(const CoreOpStart&) const = default;
};

struct CoreOpEnd {
  CoreOp op;

  auto operator<=>(const CoreOpEnd&) const = default;
};

using Event = std::variant<BusRequest, BusResponse, CoreOpStart, CoreOpEnd>;
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
