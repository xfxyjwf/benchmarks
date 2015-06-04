cmake_minimum_required(VERSION 2.8)

project(benchmarks CXX)

add_subdirectory(benchmark)
add_subdirectory(protobuf/cmake)
add_subdirectory(thrift)

add_executable(benchmarks src/benchmarks.cc)
