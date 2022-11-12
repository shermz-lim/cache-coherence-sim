#pragma once

#include <vector>
#include <optional>
#include <string_view>

enum class CoreOpLabel {
  LOAD,
  STORE,
  OTHER
};

struct CoreOp {
  size_t core_no;
  CoreOpLabel label;
  size_t value;

  auto operator<=>(const CoreOp&) const = default;
  std::string to_string();

private:
  std::string_view label_to_string();
};

struct CoreStats {
  size_t compute_cycles{0};
  size_t mem_insns{0};
  size_t idle_cycles{0};  
};

class Core {
public:
  Core(size_t core_no, const std::vector<std::pair<int, size_t>>& raw_ops);

  inline bool has_next_op() const {
    return next_op_idx < ops.size();
  }

  CoreOp next_op(size_t curr_time);
  void complete_curr_op(size_t curr_time);
  CoreStats get_stats();

  void print_state();

private:
  struct CoreOpStats {
    std::optional<size_t> start_time{};
    std::optional<size_t> end_time{};
  };

  static CoreOpLabel get_op_label(int x);

  size_t core_no;
  std::vector<CoreOp> ops;
  std::vector<CoreOpStats> ops_stats;
  size_t next_op_idx;
};
