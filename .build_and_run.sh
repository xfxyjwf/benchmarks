export CXX="g++-4.8" CC="gcc-4.8"

$CXX --version
$CC --version
cmake --version

mkdir build
cd build
make -j4 run_benchmarks
