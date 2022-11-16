#include <assert.h>
#include <iostream>

#include "cache_controller_dragon.h"

CacheControllerDragon::CacheControllerDragon(size_t core_no, Cache& cache, Bus& bus,
                                             SharedLine& shared_line)
: CacheController(core_no, cache, bus, shared_line)
{}

CacheControllerDragon::~CacheControllerDragon() = default;


bool CacheControllerDragon::handle_core_op(CoreOp op) {
  assert(op.label != CoreOpLabel::OTHER);
  CacheBlock block_no = cache.get_block_no(op.value);

  stats.tot_access++;
  if (!cache.has_block(block_no)) {
    stats.misses++;
  }

  bool read = op.label == CoreOpLabel::LOAD;
  switch (get_state(block_no)) {
    case State::INVALID:
      bus.add_request(BusTransaction{
        BusTransactionType::BUS_RD, block_no, op
      });
      return false;
    case State::EXCLUSIVE:
      if (read) {
        cache.read_block(block_no);
      } else {
        update_state(block_no, State::MODIFIED);
        cache.write_block(block_no);
      }
      stats.priv_access++;
      return true;
    case State::SHARED_CLEAN:
      if (read) {
        cache.read_block(block_no);
        stats.shared_access++;
        return true;
      } else {
        bus.add_request(BusTransaction{BusTransactionType::BUS_UPD, block_no, op});
        return false;
      }
    case State::SHARED_MODIFIED:
      if (read) {
        cache.read_block(block_no);
        stats.shared_access++;
        return true;
      } else {
        bus.add_request(BusTransaction{BusTransactionType::BUS_UPD, block_no, op});
        return false;
      }    
    case State::MODIFIED:
      if (read) {
        cache.read_block(block_no);
      } else {
        cache.write_block(block_no);
      }
      stats.priv_access++;
      return true;    
  }
}

bool CacheControllerDragon::handle_bus_resp(BusTransaction transc) {

}

bool CacheControllerDragon::handle_bus_transc(BusTransaction transc) {

}

void CacheControllerDragon::evict_block(CacheBlock block_no) {
  update_state(block_no, State::INVALID);
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
  State old_state = get_state(block_no);
  // update block's state
  if (state == State::INVALID) {
    blocks_state.erase(block_no);
  } else {
    blocks_state[block_no] = state;
  }
  // update cache
  if (old_state == State::INVALID) {
    cache.insert_block(block_no);
  } else if (state == State::INVALID) { // Shared/Exclusive/Modified -> Invalid
    bool dirty = cache.remove_block(block_no);
    assert(
      (old_state != State::MODIFIED && old_state != State::SHARED_MODIFIED)
      || dirty
    );
  }
}
