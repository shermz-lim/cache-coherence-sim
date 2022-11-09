#include <vector>
#include <optional>

enum class CoreOpLabel {
  LOAD,
  STORE,
  OTHER
};

struct CoreOp {
  CoreOpLabel label;
  size_t value;
};

struct CoreStats {
  int compute_cycles{0};
  int mem_insns{0};
  int idle_cycles{0};  
};

class Core {
public:
  Core(std::vector<std::pair<int, size_t>> raw_ops);

  inline bool has_next_op() const {
    return next_op_idx < ops.size();
  }

  CoreOp next_op(int curr_time);
  void complete_curr_op(int curr_time);
  CoreStats get_stats();

private:
  struct CoreOpStats {
    std::optional<int> start_time{};
    std::optional<int> end_time{};
  };

  static CoreOpLabel get_op_label(int x);

  std::vector<CoreOp> ops;
  std::vector<CoreOpStats> ops_stats;
  size_t next_op_idx;
};
