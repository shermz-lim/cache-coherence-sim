#include <cassert>

#include "cache_controller_imesi.h"

CacheControllerIMesi::CacheControllerIMesi(size_t core_no, Cache& cache, Bus& bus,
                                           SharedLine& shared_line)
: CacheControllerMesi(core_no, cache, bus, shared_line)
{}

BusTranscOutput CacheControllerIMesi::handle_bus_transc(BusTransaction transc) {
  CacheBlock block_no = transc.block;
  BusTransactionType t = transc.t;
  assert(transc.op_trigger.core_no != core_no);
  switch (get_state(block_no)) {
    case State::INVALID:
      return BusTranscOutput::NOTHING;
    case State::EXCLUSIVE:
      // cache-to-cache sharing
      if (t == BusTransactionType::BUS_RD) {
        update_state(block_no, State::SHARED);
        return BusTranscOutput::CACHE_TRANSFER;
      } else if (t == BusTransactionType::BUS_RDX) {
        update_state(block_no, State::INVALID);
        return BusTranscOutput::CACHE_TRANSFER;
      }
    case State::SHARED:
      // cache-to-cache sharing
      if (t == BusTransactionType::BUS_RD) {
        return BusTranscOutput::CACHE_TRANSFER;
      } else if (t == BusTransactionType::BUS_RDX) {
        update_state(block_no, State::INVALID);
        return BusTranscOutput::CACHE_TRANSFER;
      }
    case State::MODIFIED:
      if (t == BusTransactionType::BUS_RD) {
        update_state(block_no, State::SHARED);
        cache.clear_dirty_bit(block_no);
        return BusTranscOutput::FLUSH;
      } else if (t == BusTransactionType::BUS_RDX) {
        update_state(block_no, State::INVALID);
        return BusTranscOutput::FLUSH;
      }
  }
  assert(false);
  return BusTranscOutput::NOTHING;
}
