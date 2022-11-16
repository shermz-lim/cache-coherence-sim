#include <assert.h>
#include <iostream>

#include "cache_controller_mesi.h"

CacheControllerMesi::CacheControllerMesi(size_t core_no, Cache& cache, Bus& bus,
                                         SharedLine& shared_line)
: CacheController(core_no, cache, bus, shared_line)
{}

CacheControllerMesi::~CacheControllerMesi() = default;

bool CacheControllerMesi::handle_core_op(CoreOp op) {
  assert(op.label != CoreOpLabel::OTHER);
  CacheBlock block_no = cache.get_block_no(op.value);

  stats.tot_access++;
  if (!cache.has_block(block_no)) {
    stats.misses++;
  }
  
  bool read = op.label == CoreOpLabel::LOAD;
  BusTransactionType transc_t = read ? BusTransactionType::BUS_RD : BusTransactionType::BUS_RDX;
  switch (get_state(block_no)) {
    case State::INVALID:
      bus.add_request(BusTransaction{transc_t, block_no, op});
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
    case State::SHARED:
      if (read) {
        cache.read_block(block_no);
        stats.shared_access++;
        return true;
      } else {
        bus.add_request(BusTransaction{BusTransactionType::BUS_RDX, block_no, op});
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

void CacheControllerMesi::handle_bus_resp(BusTransaction transc) {
  CacheBlock block_no = transc.block;
  State state = get_state(block_no);
  assert(transc.op_trigger.core_no == core_no);
  if (state == State::INVALID && transc.t == BusTransactionType::BUS_RD) {
    State new_state = shared_line.assert_line(block_no) ? State::SHARED : State::EXCLUSIVE;
    update_state(block_no, new_state);
    cache.read_block(block_no);
    if (new_state == State::SHARED) {
      stats.shared_access++;
    } else {
      stats.priv_access++;
    }
  } else if (transc.t == BusTransactionType::BUS_RDX) {
    update_state(block_no, State::MODIFIED);
    cache.write_block(block_no);
    stats.priv_access++;
  } else {
    assert(false);
  }
}

bool CacheControllerMesi::handle_bus_transc(BusTransaction transc) {
  CacheBlock block_no = transc.block;
  BusTransactionType t = transc.t;
  assert(transc.op_trigger.core_no != core_no);
  switch (get_state(block_no)) {
    case State::INVALID:
      return false;
    case State::EXCLUSIVE:
      // no cache-to-cache sharing
      if (t == BusTransactionType::BUS_RD) {
        update_state(block_no, State::SHARED);
        return false;
      } else if (t == BusTransactionType::BUS_RDX) {
        update_state(block_no, State::INVALID);
        return false;
      }
    case State::SHARED:
      // no cache-to-cache sharing
      if (t == BusTransactionType::BUS_RD) {
        return false;
      } else if (t == BusTransactionType::BUS_RDX) {
        update_state(block_no, State::INVALID);
        return false;
      }
    case State::MODIFIED:
      if (t == BusTransactionType::BUS_RD) {
        update_state(block_no, State::SHARED);
        cache.clear_dirty_bit(block_no);
        return true;
      } else if (t == BusTransactionType::BUS_RDX) {
        update_state(block_no, State::INVALID);
        return true;
      }
  }
  assert(false);
  return false;
}

void CacheControllerMesi::evict_block(CacheBlock block_no) {
  blocks_state.erase(block_no);
}

void CacheControllerMesi::print_state() {
  std::cout << "------- MesiController " << core_no << " state -------";
  for (const auto& it : blocks_state) {
    std::cout << std::endl << it.first << ":" << state_to_string(it.second);
  }
  std::cout << std::endl;
}

std::string_view CacheControllerMesi::state_to_string(State s) {
  switch (s) {
    case State::INVALID:
      return "INVALID";
    case State::EXCLUSIVE:
      return "EXCLUSIVE";
    case State::SHARED:
      return "SHARED";
    case State::MODIFIED:
      return "MODIFIED";
  }
}

CacheControllerMesi::State CacheControllerMesi::get_state(CacheBlock block_no) {
  if (blocks_state.count(block_no) == 0) {
    return State::INVALID;
  } else {
    return blocks_state.at(block_no);
  }
}

void CacheControllerMesi::update_state(CacheBlock block_no, State state) {
  State old_state = get_state(block_no);
  assert(old_state != state);
  // update block's state
  if (state == State::INVALID) {
    blocks_state.erase(block_no);
  } else {
    blocks_state[block_no] = state;
  }
  // update cache
  if (old_state == State::INVALID) { // Invalid -> Shared/Exclusive/Modified
    cache.insert_block(block_no);
  } else if (state == State::INVALID) { // Shared/Exclusive/Modified -> Invalid
    bool dirty = cache.remove_block(block_no);
    assert(old_state != State::MODIFIED || dirty);
  }
}
