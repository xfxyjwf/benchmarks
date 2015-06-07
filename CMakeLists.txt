cmake_minimum_required(VERSION 2.8)

project(benchmarks CXX)

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(third_party/benchmark)

set(BUILD_TESTING OFF CACHE BOOL "Turn off options.")
add_subdirectory(third_party/protobuf/cmake)

# Turn off features not needed in the benchmark.
set(BUILD_EXAMPLES OFF CACHE BOOL "Turn off options.")
# set(WITH_STATIC_LIB OFF CACHE BOOL "Turn off options.")
set(WITH_STATIC_LIB ON CACHE BOOL "Turn off options.")
set(WITH_SHARED_LIB ON CACHE BOOL "Turn off options.")
set(WITH_JAVA OFF CACHE BOOL "Turn off options.")
set(WITH_PYTHON OFF CACHE BOOL "Turn off options.")
set(WITH_C_GLIB OFF CACHE BOOL "Turn off options.")
set(WITH_STDTHREADS OFF CACHE BOOL "Turn off options.")
set(WITH_BOOSTTHREADS OFF CACHE BOOL "Turn off options.")
set(WITH_ZLIB OFF CACHE BOOL "Turn off options.")
set(WITH_LIBEVENT OFF CACHE BOOL "Turn off options.")
set(WITH_OPENSSL OFF CACHE BOOL "Turn off options.")
set(WITH_QT4 OFF CACHE BOOL "Turn off options.")
set(WITH_QT5 OFF CACHE BOOL "Turn off options.")
add_subdirectory(third_party/thrift)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_command(
    OUTPUT src/generator/config.pb.cc
    DEPENDS protoc src/generator/config.proto
    COMMAND mkdir -p src
    COMMAND protoc ${PROJECT_SOURCE_DIR}/src/generator/config.proto
        --proto_path=${PROJECT_SOURCE_DIR}/src --cpp_out=src)

add_custom_command(
    OUTPUT src/proto/message.pb.cc src/generator/config.pb.cc
           src/generator/testdata.pb.cc
    DEPENDS protoc src/proto/message.proto src/generator/config.proto
            src/generator/testdata.proto
    COMMAND mkdir -p src
    COMMAND protoc --proto_path=${PROJECT_SOURCE_DIR}/src --cpp_out=src
            ${PROJECT_SOURCE_DIR}/src/proto/message.proto
            ${PROJECT_SOURCE_DIR}/src/generator/config.proto
            ${PROJECT_SOURCE_DIR}/src/generator/testdata.proto)

add_custom_command(
    OUTPUT src/thrift/message_constants.cpp src/thrift/message_types.cpp
    DEPENDS thrift-compiler src/thrift/message.thrift
    COMMAND mkdir -p src/thrift
    COMMAND thrift-compiler --gen cpp -out src/thrift
        ${PROJECT_SOURCE_DIR}/src/thrift/message.thrift)

include_directories(
    ${PROJECT_BINARY_DIR}/src
    ${PROJECT_BINARY_DIR}/third_party/protobuf/cmake
    ${PROJECT_BINARY_DIR}/third_party/thrift
    src
    third_party/benchmark/include
    third_party/protobuf/src
    third_party/thrift/lib/cpp/src)

add_library(message_definitions
    src/generator/config.pb.cc
    src/generator/testdata.pb.cc
    src/proto/message.pb.cc
    src/thrift/message_constants.cpp
    src/thrift/message_types.cpp)

target_link_libraries(message_definitions libprotobuf thrift_static)

add_executable(generator src/generator/generator.cc)

target_link_libraries(generator message_definitions libprotobuf thrift_static)

add_executable(benchmarks src/benchmarks.cc)

target_link_libraries(benchmarks message_definitions benchmark libprotobuf
                      thrift_static)

set(benchmark_list)
macro(add_benchmark test_name)
  add_custom_target(run_${test_name}
      DEPENDS generator benchmarks src/testdata/${test_name}.txt
      COMMAND generator ${PROJECT_SOURCE_DIR}/src/testdata/${test_name}.txt ${test_name}.dat
      COMMAND benchmarks < ${test_name}.dat)
  list(APPEND benchmark_list run_${test_name})
endmacro(add_benchmark)

add_benchmark(test1)
add_benchmark(test2)
add_benchmark(test3)
add_benchmark(test4)
add_custom_target(run_benchmarks DEPENDS ${benchmark_list})
