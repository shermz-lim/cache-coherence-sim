'''
Runs experiments based on different configurations
'''
import subprocess
import os

DEF_CACHE_SIZE = 4096;
DEF_ASSOC = 2;
DEF_BLOCK_SIZE = 32;

cache_sizes = [2**i for i in range(9, 19)]
assocs = [2**i for i in range(8)]
block_sizes = [2**i for i in range(2, 10)]

protocols = [
  "MESI", "IMESI", "Dragon"
]
inputs = [
  "blackscholes", "bodytrack", "fluidanimate"
]

def run_experiment(protocol, inp, cs, a, blk_sz):
  if (cs / a / blk_sz) < 1:
    return
  out_path = os.path.join("out", f"{protocol}_{inp}_{cs}_{a}_{blk_sz}.out")
  with open(out_path, "w+") as out_file:
    subprocess.call(
      map(str, ["./coherence", protocol, inp, cs, a, blk_sz]),
      stdout=out_file
    )

# run_experiment("MESI", "blackscholes", DEF_CACHE_SIZE, DEF_ASSOC, DEF_BLOCK_SIZE)
for cs in cache_sizes:
  for protocol in protocols:
    for inp in inputs:
      run_experiment(protocol, inp, cs, DEF_ASSOC, DEF_BLOCK_SIZE)
for a in assocs:
  for protocol in protocols:
    for inp in inputs:
      run_experiment(protocol, inp, DEF_CACHE_SIZE, a, DEF_BLOCK_SIZE)
for blk_sz in block_sizes:
  for protocol in protocols:
    for inp in inputs:
      run_experiment(protocol, inp, DEF_CACHE_SIZE, DEF_ASSOC, blk_sz)
