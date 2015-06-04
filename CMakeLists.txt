cmake_minimum_required(VERSION 2.8)

project(benchmarks CXX)

add_subdirectory(benchmark)
add_subdirectory(protobuf/cmake)
add_subdirectory(thrift)

add_custom_command(
    OUTPUT message.pb.cc
    COMMAND protoc ${CMAKE_SOURCE_DIR}/src/proto/message.proto
        --proto_path=${CMAKE_SOURCE_DIR}/src/proto --cpp_out=${CMAKE_BINARY_DIR}
    DEPENDS protoc src/proto/message.proto)

add_custom_command(
    OUTPUT message_constants.cpp message_types.cpp
    COMMAND bin/thrift --gen cpp -out ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/src/thrift/message.thrift
    DEPENDS thrift-compiler src/thrift/message.thrift)

include_directories(benchmark/include protobuf/src thrift/lib/cpp/src
    ${CMAKE_BINARY_DIR}/protobuf/cmake
    ${CMAKE_BINARY_DIR}/thrift)

add_executable(benchmarks src/benchmarks.cc
    ${CMAKE_BINARY_DIR}/message.pb.cc
    ${CMAKE_BINARY_DIR}/message_constants.cpp
    ${CMAKE_BINARY_DIR}/message_types.cpp)

target_link_libraries(benchmarks benchmark libprotobuf thrift)
