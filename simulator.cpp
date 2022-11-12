#include "simulator.h"

Simulator::Simulator(std::vector<Core>& cores, std::vector<Cache>& caches,
                     Bus& bus, SharedLine& shared_line,
                     std::vector<std::unique_ptr<CacheController>>& cache_controllers)
: cores(cores), caches(caches), bus(bus), shared_line(shared_line),
  cache_controllers(cache_controllers)
{}

void Simulator::simulate() {
  size_t curr_clock = 0;
  EventQueue events{};
  // init events
  for (auto& core : cores) {
    events.insert(std::make_pair(
      curr_clock,
      CoreOpStart{core.next_op(curr_clock)}
    ));
  }
  // run simulation
  while (!events.empty()) {
    
  }
}
