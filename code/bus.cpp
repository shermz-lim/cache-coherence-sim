#include <iostream>
#include <sstream>

#include "bus.h"

std::string BusTransaction::to_string() {
  std::stringstream ss{};
  ss << "BusTransaction{" << type_to_string()
     << "," << block
     << "," << op_trigger.to_string()
     << "}";
  return ss.str();
}

std::string_view BusTransaction::type_to_string() {
  switch (t) {
    case BusTransactionType::BUS_RD:
      return "BUS_RD";
    case BusTransactionType::BUS_RDX:
      return "BUS_RDX";
    case BusTransactionType::BUS_WB:
      return "BUS_WB";
    case BusTransactionType::BUS_UPD:
      return "BUS_UPD";
  }
}

void Bus::curr_transc_complete(BusTransaction transc) {
  CacheBlock block_no = transc.block;
  assert(curr_transcs.at(block_no) == transc);
  curr_transcs.erase(block_no);
  switch (transc.t) {
    case BusTransactionType::BUS_RD:
    case BusTransactionType::BUS_WB:
      stats.blks_traffic++;
      break;
    case BusTransactionType::BUS_RDX:
    case BusTransactionType::BUS_UPD:
      stats.blks_traffic++;
      stats.num_inv_upds++;
      break;
  }
}

std::vector<BusTransaction> Bus::next_transcs() {
  if (curr_transcs.size() >= NUM_CONCURRENT_TRANSC) {
    return std::vector<BusTransaction>{};
  }
  std::vector<BusTransaction> nxt;
  for (auto& it : requests) {
    if (curr_transcs.count(it.first)) continue;
    BusTransaction transc = it.second.front();
    it.second.pop_front();
    curr_transcs[it.first] = transc;
    nxt.push_back(transc);
  }
  for (auto& transc : nxt) {
    if (requests.at(transc.block).empty()) requests.erase(transc.block);
  }
  return nxt;
}

void Bus::print_state() {
  std::cout << "------- Bus state -------\n";
  std::cout << "current:\n";
  for (auto& it : curr_transcs) {
    std::cout << it.first << ": " << it.second.to_string() << std::endl;
  }
  std::cout << "requests:\n";
  for (auto& it : requests) {
    std::cout << it.first << ": ";
    for (auto& transc : it.second) {
      std::cout << transc.to_string() << "; ";
    }
    std::cout << std::endl;
  }  
}
