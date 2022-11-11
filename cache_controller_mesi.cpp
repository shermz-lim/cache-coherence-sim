#include <assert.h>

#include "cache_controller_mesi.h"

CacheControllerMesi::CacheControllerMesi(Cache& cache, Bus& bus,
                                         SharedLine& shared_line)
: CacheController(cache, bus, shared_line)
{}

CacheControllerMesi::~CacheControllerMesi() = default;

void CacheControllerMesi::handle_core_op(CoreOp op) {
  assert(op.label != CoreOpLabel::OTHER);

  CacheBlock block_no = cache.get_block_no(op.value);
  bool read = op.label == CoreOpLabel::LOAD;
  BusTransactionType transc_t = read ? BusTransactionType::BUS_RD : BusTransactionType::BUS_RDX;
  switch (get_state(block_no)) {
    case State::INVALID:
      bus.add_request(BusTransaction{transc_t, block_no, op});
      return;
    case State::EXCLUSIVE:
      if (read) {
        cache.read_block(block_no);
      } else {
        update_state(block_no, State::MODIFIED);
        cache.write_block(block_no);
      }
      return;
    case State::SHARED:
      if (read) {
        cache.read_block(block_no);
      } else {
        bus.add_request(BusTransaction{BusTransactionType::BUS_UPGR, block_no, op});
        update_state(block_no, State::MODIFIED);
        cache.write_block(block_no);
      }
      return;
    case State::MODIFIED:
      if (read) {
        cache.read_block(block_no);
      } else {
        cache.write_block(block_no);
      }
      return;
  }
}

void CacheControllerMesi::handle_bus_resp(BusTransaction transc) {
  CacheBlock block_no = transc.block;
  State state = get_state(block_no);
  if (state == State::INVALID && transc.t == BusTransactionType::BUS_RD) {
    State new_state = shared_line.assert_line(block_no) ? State::SHARED : State::EXCLUSIVE;
    update_state(block_no, new_state);
    cache.read_block(block_no);
  } else if (state == State::INVALID && transc.t == BusTransactionType::BUS_RDX) {
    update_state(block_no, State::MODIFIED);
    cache.write_block(block_no);
  } else {
    assert(false);
  }
}

bool CacheControllerMesi::handle_bus_transc(BusTransaction transc) {
  CacheBlock block_no = transc.block;
  BusTransactionType t = transc.t;
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
      } else if (t == BusTransactionType::BUS_RDX || t == BusTransactionType::BUS_UPGR) {
        update_state(block_no, State::INVALID);
        return false;
      }
    case State::MODIFIED:
      if (t == BusTransactionType::BUS_RD) {
        update_state(block_no, State::SHARED);
        return true;
      } else if (t == BusTransactionType::BUS_RDX) {
        update_state(block_no, State::INVALID);
        return true;
      }
  }
  assert(false);
  return false;
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
  blocks_state[block_no] = state;
  // update cache
  if (old_state == State::INVALID) { // Invalid -> Shared/Exclusive/Modified
    cache.insert_block(block_no);
  } else if (state == State::INVALID) { // Shared/Exclusive/Modified -> Invalid
    bool dirty = cache.remove_block(block_no);
    assert(old_state != State::MODIFIED || dirty);
  }
}
