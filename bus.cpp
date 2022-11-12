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
    case BusTransactionType::BUS_UPGR:
      return "BUS_UPGR";
  }
}

BusTransaction Bus::next_transc() {
  BusTransaction transc = requests.front();
  requests.pop_front();
  curr_transc = transc;
  return transc;
}

void Bus::print_state() {
  std::cout << "------- Bus state -------\n";
  std::cout << "curr: "
            << (curr_transc.has_value() ? curr_transc.value().to_string() : "")
            << "\nrequests: ";
  for (auto& transc : requests) {
    std::cout << transc.to_string() << "; ";
  }
  std::cout << std::endl;
}
