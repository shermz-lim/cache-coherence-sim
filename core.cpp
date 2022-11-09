#include "core.h"

Core::Core(std::vector<std::pair<int, size_t>> raw_ops)
: ops(raw_ops.size()),
  ops_stats(raw_ops.size()),
  next_op_idx{0}
{
  for (size_t i = 0; i < raw_ops.size(); i++) {
    auto& raw_op = raw_ops.at(i);
    CoreOp& op = ops.at(i);
    op.label = get_op_label(raw_op.first);
    op.value = raw_op.second;
  }
}

CoreOp Core::next_op(int curr_time) {
  CoreOp& op = ops.at(next_op_idx);
  ops_stats.at(next_op_idx).start_time = curr_time;
  next_op_idx++;
  return op;
}

void Core::complete_curr_op(int curr_time) {
  ops_stats.at(next_op_idx - 1).end_time = curr_time;
}

CoreStats Core::get_stats() {
  CoreStats stats{};
  for (size_t i = 0; i < next_op_idx; i++) {
    CoreOp& op = ops.at(i);
    CoreOpStats& op_stats = ops_stats.at(i);
    int op_cycles = op_stats.end_time.value() - op_stats.start_time.value();
    switch (op.label) {
      case CoreOpLabel::LOAD:
      case CoreOpLabel::STORE:
        stats.mem_insns++;
        stats.idle_cycles += op_cycles;
        continue;
      case CoreOpLabel::OTHER:
        stats.compute_cycles += op_cycles;
        continue;
    }
  }
  return stats;
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
