#include "simulator.h"

Simulator::Simulator(std::vector<Core>& cores, std::vector<Cache>& caches,
                     Bus& bus, SharedLine& shared_line,
                     std::vector<std::unique_ptr<CacheController>>& cache_controllers)
: cores(cores), caches(caches), bus(bus), shared_line(shared_line),
  cache_controllers(cache_controllers)
{}

void Simulator::simulate() {
  EventQueue events{};
  while (!events.empty()) {
    
  }
}
