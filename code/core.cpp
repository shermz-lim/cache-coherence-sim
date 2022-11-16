#include <assert.h>
#include <iostream>
#include <sstream>

#include "core.h"

std::string CoreOp::to_string() {
  std::stringstream ss{};
  ss << "CoreOp{" << core_no 
     << "," << label_to_string()
     << "," << value
     << "}";
  return ss.str();
}

std::string_view CoreOp::label_to_string() {
  switch (label) {
    case CoreOpLabel::LOAD:
      return "LOAD";
    case CoreOpLabel::STORE:
      return "STORE";
    case CoreOpLabel::OTHER:
      return "OTHER";
  }
}

Core::Core(size_t core_no, const std::vector<std::pair<int, size_t>>& raw_ops)
: core_no(core_no),
  ops(raw_ops.size()),
  ops_stats(raw_ops.size()),
  next_op_idx{0}
{
  for (size_t i = 0; i < raw_ops.size(); i++) {
    auto& raw_op = raw_ops.at(i);
    CoreOp& op = ops.at(i);
    op.core_no = core_no;
    op.label = get_op_label(raw_op.first);
    op.value = raw_op.second;
  }
}

CoreOp Core::next_op(size_t curr_time) {
  CoreOp& op = ops.at(next_op_idx);
  ops_stats.at(next_op_idx).start_time = curr_time;
  next_op_idx++;
  return op;
}

void Core::complete_curr_op(size_t curr_time) {
  ops_stats.at(next_op_idx - 1).end_time = curr_time;
}

CoreStats Core::get_stats() {
  assert(next_op_idx == ops.size());

  CoreStats stats{};
  stats.exec_cycles = ops_stats.back().end_time.value();

  for (size_t i = 0; i < next_op_idx; i++) {
    CoreOp& op = ops.at(i);
    CoreOpStats& op_stats = ops_stats.at(i);
    size_t op_cycles = op_stats.end_time.value() - op_stats.start_time.value();
    switch (op.label) {
      case CoreOpLabel::LOAD:
        stats.load_insns++;
        stats.idle_cycles += op_cycles;
        continue;
      case CoreOpLabel::STORE:
        stats.store_insns++;
        stats.idle_cycles += op_cycles;
        continue;
      case CoreOpLabel::OTHER:
        assert(op_cycles == op.value);
        stats.compute_cycles += op_cycles;
        continue;
    }
  }

  return stats;
}

void Core::print_state() {
  assert(next_op_idx > 0);
  size_t i = next_op_idx - 1;
  std::cout << "------- Core " << core_no << " state -------\n";
  std::cout << "curr_op: " << ops.at(i).to_string()
            << "; start_time: " << ops_stats.at(i).start_time.value()
            << std::endl;
}

CoreOpLabel Core::get_op_label(int x) {
  switch (x) {
    case 0:
      return CoreOpLabel::LOAD;
    case 1:
      return CoreOpLabel::STORE;
    case 2:
      return CoreOpLabel::OTHER;
    default:
      throw std::runtime_error{"invalid core op label int"};
  }
}
