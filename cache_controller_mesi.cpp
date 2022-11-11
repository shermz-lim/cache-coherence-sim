#include <assert.h>

#include "cache_controller_mesi.h"

CacheControllerMesi::CacheControllerMesi(Cache& cache, Bus& bus,
                                         SharedLine& shared_line,
                                         size_t core_idx)
: CacheController(cache, bus, shared_line, core_idx)
{}

CacheControllerMesi::~CacheControllerMesi() = default;

void CacheControllerMesi::handle_core_op(CoreOp op) {
  assert(op.label != CoreOpLabel::OTHER);

  CacheBlock block_no = cache.get_block_no(op.value);
  bool read = op.label == CoreOpLabel::LOAD;
  BusTransactionType transc_t = read ? BusTransactionType::BUS_RD : BusTransactionType::BUS_RDX;
  switch (get_state(block_no)) {
    case State::INVALID:
      bus.add_request(BusTransaction{transc_t, core_idx, block_no});
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
        bus.add_request(BusTransaction{BusTransactionType::BUS_UPGR, core_idx, block_no});
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
    cache.insert_block(block_no);
    cache.read_block(block_no);
    State new_state = shared_line.assert_line(block_no) ? State::SHARED : State::EXCLUSIVE;
    update_state(block_no, new_state);
  } else if (state == State::INVALID && transc.t == BusTransactionType::BUS_RDX) {
    cache.insert_block(block_no);
    cache.write_block(block_no);
    update_state(block_no, State::MODIFIED);
  } else {
    assert(false);
  }
}

bool CacheControllerMesi::handle_bus_transc(BusTransaction transc) {
  
}

CacheControllerMesi::State CacheControllerMesi::get_state(CacheBlock block_no) {
  if (blocks_state.count(block_no) == 0) {
    return State::INVALID;
  } else {
    return blocks_state.at(block_no);
  }
}

void CacheControllerMesi::update_state(CacheBlock block_no, State state) {
  blocks_state[block_no] = state;
}
