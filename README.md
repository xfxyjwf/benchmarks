# Benchmarks for Protocol Buffers and Thrift.

## How to run the benchmark

  1. Clone the repository:

        $ git clone https://github.com/xfxyjwf/benchmarks.git
        $ cd benchmarks
        $ git submodule init
        $ git submodule update

  2. Install dependencies of Thrift following instructions here:

        https://thrift.apache.org/docs/install/debian

  3. Bulid and run the benchmark:

        $ mkdir build
        $ cd build
        $ cmake ..
        $ make -j benchmarks
        $ ./benchmarks

