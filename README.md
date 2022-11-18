# Cache Coherence Simulator

Cache Coherence Simulator is a software simulator for a multi-core system with private L1 data caches and a single shared memory, with cache coherence implemented.
It is used to explore the designs of various cache coherence protocols, namely MESI, Dragon, and Illinois MESI.

## Setup

The compiler `Apple clang version 14.0.0` is required.

## Usage

Unzip the benchmark traces in `data`:
```
make seed
```

Build the simulator:
```
make
```

Run the simulator using MESI protocol, bodytrack benchmark, and 1KB direct-mapped cache with 16B block size
```
./coherence MESI bodytrack 1024 1 16
```

To build the simulator for debugging:
```
make debug
```

To clean up:
```
make clean
```
