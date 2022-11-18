'''
Plot results of experiments
'''

import csv
from benchmark import *

def get_value(line):
  return line.split(":")[1].strip()

def parse_exp_result(protocol, inp, cs, a, blk_sz):
  out_path = os.path.join("out", f"{protocol}_{inp}_{cs}_{a}_{blk_sz}.out")
  with open(out_path) as f:
    lines = f.readlines()
    return [
      float(get_value(lines[14])[:-1]),
      float(get_value(lines[25])[:-1]),
      float(get_value(lines[36])[:-1]),
      float(get_value(lines[47])[:-1]),
      int(get_value(lines[51])),
      int(get_value(lines[52])),
      int(get_value(lines[53])),
      int(get_value(lines[54])),
      int(get_value(lines[55]))
    ]

def parse():
  experiments = []
  for cs in cache_sizes:
    for protocol in protocols:
      for inp in inputs:
        experiments.append((protocol, inp, cs, DEF_ASSOC, DEF_BLOCK_SIZE))
  for a in assocs:
    for protocol in protocols:
      for inp in inputs:
        experiments.append((protocol, inp, DEF_CACHE_SIZE, a, DEF_BLOCK_SIZE))
  for blk_sz in block_sizes:
    for protocol in protocols:
      for inp in inputs:
        experiments.append((protocol, inp, DEF_CACHE_SIZE, DEF_ASSOC, blk_sz))
  rows = [['protocol', 'benchmark', 'cache_size', 'associativity', 'block_size',
          'miss_rate_0', 'miss_rate_1', 'miss_rate_2', 'miss_rate_3',
          'execution_cycles', 'data_traffic', 'num_inv_upd', 'priv_access',
          'shared_access']]
  for protocol, inp, cs, a, blk_sz in experiments:
    row = [protocol, inp, cs, a, blk_sz]
    row += parse_exp_result(protocol, inp, cs, a, blk_sz)
    rows.append(row)
  with open(os.path.join("experiments", "benchmark.csv"), "w") as f:
    writer = csv.writer(f)
    writer.writerows(rows)

def main():
  pass

if __name__ == "__main__":
  parse()
