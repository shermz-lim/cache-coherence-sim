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
  CacheBlock block_no = transc.block;
  State state = get_state(block_no);
  assert(transc.op_trigger.core_no == core_no);
  if (state == State::INVALID && transc.t == BusTransactionType::BUS_RD) {
    bool shared = shared_line.assert_line(block_no, core_no);
    if (transc.op_trigger.label == CoreOpLabel::LOAD) {
      // read miss
      update_state(block_no, shared ? State::SHARED_CLEAN : State::EXCLUSIVE);
      cache.read_block(block_no);
      if (shared) {
        stats.shared_access++;
      } else {
        stats.priv_access++;
      }
      return true;
    } else if (transc.op_trigger.label == CoreOpLabel::STORE) {
      // write miss
      if (shared) {
        update_state(block_no, State::SHARED_CLEAN);
        cache.read_block(block_no);
        bus.add_request(
          BusTransaction{BusTransactionType::BUS_UPD, block_no, transc.op_trigger}
        );
        return false;
      } else {
        update_state(block_no, State::MODIFIED);
        cache.write_block(block_no);
        stats.priv_access++;
        return true;
      }
    }

  } else if (
    (state == State::SHARED_CLEAN || state == State::SHARED_MODIFIED)
    && transc.t == BusTransactionType::BUS_UPD
  ) {
    bool shared = shared_line.assert_line(block_no, core_no);
    update_state(block_no, shared ? State::SHARED_MODIFIED : State::MODIFIED);
    cache.write_block(block_no);
    if (shared) {
      stats.shared_access++;
    } else {
      stats.priv_access++;
    }
    return true;

  }
  assert(false);
  return false;
}

BusTranscOutput CacheControllerDragon::handle_bus_transc(BusTransaction transc) {
  CacheBlock block_no = transc.block;
  BusTransactionType t = transc.t;
  assert(transc.op_trigger.core_no != core_no);
  switch (get_state(block_no)) {
    case State::INVALID:
      return BusTranscOutput::NOTHING;
    case State::EXCLUSIVE:
      if (t == BusTransactionType::BUS_RD) {
        update_state(block_no, State::SHARED_CLEAN);
        return BusTranscOutput::NOTHING;
      }
    case State::SHARED_CLEAN:
      if (t == BusTransactionType::BUS_RD || t == BusTransactionType::BUS_UPD) {
        return BusTranscOutput::NOTHING;
      }
    case State::SHARED_MODIFIED:
      if (t == BusTransactionType::BUS_RD) {
        return BusTranscOutput::FLUSH;
      } else if (t == BusTransactionType::BUS_UPD) {
        update_state(block_no, State::SHARED_CLEAN);
        cache.clear_dirty_bit(block_no);
        return BusTranscOutput::NOTHING;
      }
    case State::MODIFIED:
      if (t == BusTransactionType::BUS_RD) {
        update_state(block_no, State::SHARED_MODIFIED);
        return BusTranscOutput::FLUSH;
      }
  }
  assert(false);
  return BusTranscOutput::NOTHING;
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
