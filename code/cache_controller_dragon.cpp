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
}

void CacheControllerDragon::print_state() {
}

std::string_view CacheControllerDragon::state_to_string(State s) {
}

CacheControllerDragon::State CacheControllerDragon::get_state(CacheBlock block_no) {
}

void CacheControllerDragon::update_state(CacheBlock block_no, State state) {
}
