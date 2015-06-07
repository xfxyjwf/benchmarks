mkdir build
cd build
cmake --version
cmake -DBUILD_TESTING=OFF ..
make -j4 benchmarks
./benchmarks
