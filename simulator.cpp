#include <iostream>

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
  EventHandler event_handler{*this};
  EventString event_string{*this};
  while (!events.empty()) {
    // obtain earliest event
    auto time_event = *events.begin();
    size_t time = time_event.first;
    Event event = time_event.second;
    events.erase(events.begin());

    // start of logging
    std::cout << "========= Event Processing =========\n"
              << "time: " << time
              << ", " << std::visit(event_string, event)
              << std::endl;
    bus.print_state();
    for (auto& core : cores) core.print_state();
    for (auto& cache : caches) cache.print_state();
    for (auto& controller : cache_controllers) controller->print_state();
    std::cout << std::endl;
    // end of logging

  }
}

Simulator::EventHandler::EventHandler(Simulator& sim)
: sim(sim)
{}

void Simulator::EventHandler::operator()(BusRequest&) {
}

void Simulator::EventHandler::operator()(BusResponse&) {
}

void Simulator::EventHandler::operator()(CoreOpStart&) {
}

void Simulator::EventHandler::operator()(CoreOpEnd&) {
}

Simulator::EventString::EventString(Simulator& sim)
: sim(sim)
{}

std::string Simulator::EventString::operator()(BusRequest& req) {
  return "BusRequest: " + req.transc.to_string();
}

std::string Simulator::EventString::operator()(BusResponse& res) {
  return "BusResponse: " + res.transc.to_string();
}

std::string Simulator::EventString::operator()(CoreOpStart& op_s) {
  return "CoreOpStart: " + op_s.op.to_string();
}

std::string Simulator::EventString::operator()(CoreOpEnd& op_e) {
  return "CoreOpEnd: " + op_e.op.to_string();
}
