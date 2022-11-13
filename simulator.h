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
  struct EventHandler {
    EventHandler(Simulator& sim);
    void operator()(BusRequest&);
    void operator()(BusResponse&);
    void operator()(CoreOpStart&);
    void operator()(CoreOpEnd&);

    Simulator& sim;
  };

  struct EventString {
    EventString(Simulator& sim);
    std::string operator()(BusRequest&);
    std::string operator()(BusResponse&);
    std::string operator()(CoreOpStart&);
    std::string operator()(CoreOpEnd&);

    Simulator& sim;
  };

  size_t curr_clock{0};
  EventQueue events{};
  std::vector<Core>& cores;
  std::vector<Cache>& caches;
  Bus& bus;
  SharedLine& shared_line;
  std::vector<std::unique_ptr<CacheController>>& cache_controllers;
};
