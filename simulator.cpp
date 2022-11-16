#include <iostream>

#include "simulator.h"

Simulator::Simulator(std::vector<Core>& cores, std::vector<Cache>& caches,
                     Bus& bus, SharedLine& shared_line,
                     std::vector<std::unique_ptr<CacheController>>& cache_controllers)
: cores(cores), caches(caches), bus(bus), shared_line(shared_line),
  cache_controllers(cache_controllers)
{}

void Simulator::simulate() {
  // init events
  for (auto& core : cores) {
    add_event(curr_clock, CoreOpStart{core.next_op(curr_clock)});
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

    // event processing
    curr_clock = time;
    std::visit(event_handler, event);

    // bus arbiter: get next bus transaction if bus is free
    if (!bus.has_curr_transc() && bus.has_request()) {
      auto transc = bus.next_transc();
      add_event(curr_clock, BusRequest{transc});
    }
  }
}

Simulator::EventHandler::EventHandler(Simulator& sim)
: sim(sim)
{}

void Simulator::EventHandler::operator()(BusRequest& req) {
  auto& transc = req.transc;
  size_t resp_cycles = 0;

  if (transc.t == BusTransactionType::BUS_WB) {
    resp_cycles += MEM_ACCESS_TIME;

  } else { // a read
    bool flush = false;
    for (size_t core_no = 0; core_no < sim.cores.size(); core_no++) {
      if (transc.op_trigger.core_no == core_no) continue;
      bool flush_i = sim.cache_controllers.at(core_no)->handle_bus_transc(
        transc
      );
      flush = flush || flush_i;
    }

    // time to flush or not + time to read
    resp_cycles += ((flush ? MEM_ACCESS_TIME : 0) + MEM_ACCESS_TIME);
  }

  sim.add_event(sim.curr_clock + resp_cycles, BusResponse{transc});
}

void Simulator::EventHandler::operator()(BusResponse& resp) {
  sim.bus.curr_transc_complete();
  auto& transc = resp.transc;
  if (transc.t == BusTransactionType::BUS_WB) { // eviction successful
    // restart operation
    sim.add_event(sim.curr_clock, CoreOpStart{transc.op_trigger});
  } else { // bus read complete
    sim.cache_controllers.at(transc.op_trigger.core_no)->handle_bus_resp(
      transc
    );
    // 1 cycle for cache hit
    sim.add_event(sim.curr_clock + CACHE_HIT_TIME, CoreOpEnd{transc.op_trigger});
  }
}

void Simulator::EventHandler::operator()(CoreOpStart& op_s) {
  CoreOp& op = op_s.op;
  if (op.label == CoreOpLabel::OTHER) { // compute operations
    sim.add_event(sim.curr_clock + op.value, CoreOpEnd{op});
    return;
  }

  // memory operations
  auto core_no = op.core_no;
  auto& cache = sim.caches.at(core_no);
  auto& controller = *sim.cache_controllers.at(core_no);
  CacheBlock block_no = cache.get_block_no(op.value);

  // handles cache miss and block eviction
  if (!cache.has_block(block_no)) {
    // maybe need evict block
    auto evicted_block = cache.maybe_evict_block(block_no);
    if (evicted_block.has_value()) {
      controller.evict_block(evicted_block->first);
      if (evicted_block->second) { // block is dirty, need write back
        sim.bus.add_request(
          BusTransaction{BusTransactionType::BUS_WB, evicted_block->first, op}
        );
        return;
      }
      // evicted block is clean, continue
    }
  }

  bool done = controller.handle_core_op(op);
  if (done) {
    // 1 cycle for cache hit
    sim.add_event(sim.curr_clock + CACHE_HIT_TIME, CoreOpEnd{op});
  }
}

void Simulator::EventHandler::operator()(CoreOpEnd& op_e) {
  CoreOp& op = op_e.op;
  Core& core = sim.cores.at(op.core_no);
  core.complete_curr_op(sim.curr_clock);
  if (core.has_next_op()) {
    sim.add_event(sim.curr_clock, CoreOpStart{core.next_op(sim.curr_clock)});
  }
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
