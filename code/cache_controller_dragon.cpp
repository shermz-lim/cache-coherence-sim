#include <assert.h>
#include <iostream>

#include "cache_controller_dragon.h"

CacheControllerDragon::CacheControllerDragon(size_t core_no, Cache& cache, Bus& bus,
                                             SharedLine& shared_line)
: CacheController(core_no, cache, bus, shared_line)
{}

CacheControllerDragon::~CacheControllerDragon() = default;


bool CacheControllerDragon::handle_core_op(CoreOp op) {

}

void CacheControllerDragon::handle_bus_resp(BusTransaction transc) {

}

bool CacheControllerDragon::handle_bus_transc(BusTransaction transc) {

}

void CacheControllerDragon::evict_block(CacheBlock block_no) {
  blocks_state.erase(block_no);
}

void CacheControllerDragon::print_state() {
  std::cout << "------- DragonController " << core_no << " state -------";
  for (const auto& it : blocks_state) {
    std::cout << std::endl << it.first << ":" << state_to_string(it.second);
  }
  std::cout << std::endl;
}

std::string_view CacheControllerDragon::state_to_string(State s) {
  switch (s) {
    case State::INVALID:
      return "INVALID";
    case State::EXCLUSIVE:
      return "EXCLUSIVE";
    case State::SHARED_CLEAN:
      return "SHARED_CLEAN";
    case State::SHARED_MODIFIED:
      return "SHARED_MODIFIED";
    case State::MODIFIED:
      return "MODIFIED";
  }
}

CacheControllerDragon::State CacheControllerDragon::get_state(CacheBlock block_no) {
  if (blocks_state.count(block_no) == 0) {
    return State::INVALID;
  } else {
    return blocks_state.at(block_no);
  }
}

void CacheControllerDragon::update_state(CacheBlock block_no, State state) {
}
