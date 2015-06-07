cmake_minimum_required(VERSION 2.8)

project(benchmarks CXX)

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(third_party/benchmark)

set(BUILD_TESTING OFF)
add_subdirectory(third_party/protobuf/cmake)

# Turn off features not needed in the benchmark.
set(BUILD_EXAMPLES OFF)
set(WITH_STATIC_LIB OFF)
set(WITH_JAVA OFF)
set(WITH_PYTHON OFF)
set(WITH_C_GLIB OFF)
set(WITH_STDTHREADS OFF)
set(WITH_BOOSTTHREADS OFF)
set(WITH_ZLIB OFF)
set(WITH_LIBEVENT OFF)
set(WITH_OPENSSL OFF)
add_subdirectory(third_party/thrift)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_command(
    OUTPUT src/proto/message.pb.cc src/proto/message.pb.h
    DEPENDS protoc src/proto/message.proto
    COMMAND mkdir -p src
    COMMAND protoc ${PROJECT_SOURCE_DIR}/src/proto/message.proto
        --proto_path=${PROJECT_SOURCE_DIR}/src --cpp_out=src)

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

add_executable(benchmarks
    src/benchmarks.cc
    src/proto/message.pb.cc
    src/thrift/message_constants.cpp
    src/thrift/message_types.cpp)

target_link_libraries(benchmarks benchmark libprotobuf thrift)
