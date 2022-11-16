#pragma once

#include <set>

#include "cache_controller.h"

const size_t CACHE_HIT_TIME = 1;
const size_t MEM_ACCESS_TIME = 100;

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

  inline void add_event(size_t time, const Event& e) {
    events.insert(std::make_pair(time, e));
  }

  size_t curr_clock{0};
  EventQueue events{};
  std::vector<Core>& cores;
  std::vector<Cache>& caches;
  Bus& bus;
  SharedLine& shared_line;
  std::vector<std::unique_ptr<CacheController>>& cache_controllers;
};
