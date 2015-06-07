#include <cstdint>
#include <string>
#include <iostream>

#include "benchmark/benchmark.h"
#include "proto/message.pb.h"
#include "generator/testdata.pb.h"
#include "thrift/message_types.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/transport/TBufferTransports.h"

namespace benchmarks {
namespace {
using std::string;
using std::cout;
using std::endl;
using apache::thrift::transport::TMemoryBuffer;
using apache::thrift::protocol::TBinaryProtocol;

testdata::DataSet dataset_;

void InitializeData(testdata::DataSet&& dataset) {
  dataset_.Swap(&dataset);
}

string GetProtoAllTypes() {
  return dataset_.protobuf_data();
}

string GetThriftAllTypes() {
  return dataset_.thrift_data();
}

void PrintInformation() {
  cout << "Proto size: " << GetProtoAllTypes().size() << endl;
  cout << "Thrift size: " << GetThriftAllTypes().size() << endl;
  // proto::TestAllTypes message;
  // message.ParseFromString(GetProtoAllTypes());
  // cout << "Message structure: " << endl;
  // cout << "============================" << endl;
  // cout << message.DebugString() << endl;
  // cout << "============================" << endl;
}

// Do everything except parsing. Offset the CPU time result of this benchmark
// from the result of BM_Proto_ParseAllTypes to get the actual CPU time used
// in parsing.
void BM_Proto_ParseAllTypes_Empty(benchmark::State& state) {
  string data = GetProtoAllTypes();
  int64_t total_bytes = 0;
  while (state.KeepRunning()) {
    proto::TestAllTypes message;
    if (message.int32_value() != 0) {
      throw "invalid message";
    }
    total_bytes += data.size();
  }
  // state.SetBytesProcessed(total_bytes);
}
BENCHMARK(BM_Proto_ParseAllTypes_Empty);

void BM_Proto_ParseAllTypes(benchmark::State& state) {
  string data = GetProtoAllTypes();
  int64_t total_bytes = 0;
  while (state.KeepRunning()) {
    proto::TestAllTypes message;
    if (!message.ParseFromString(data)) {
      throw "proto parsing failed.";
    }
    total_bytes += data.size();
  }
  state.SetBytesProcessed(total_bytes);
}
BENCHMARK(BM_Proto_ParseAllTypes);

// Do everything except parsing. Offset the CPU time result of this benchmark
// from the result of BM_Thrift_ParseAllTypes to get the actual CPU time used
// in parsing.
void BM_Thrift_ParseAllTypes_Empty(benchmark::State& state) {
  string data = GetThriftAllTypes();
  int64_t total_bytes = 0;
  while (state.KeepRunning()) {
    thrift::TestAllTypes message;
    boost::shared_ptr<TMemoryBuffer> buffer(new TMemoryBuffer(
          reinterpret_cast<uint8_t*>(const_cast<char*>(data.data())), data.length()));
    boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(buffer));
    uint32_t length = buffer->readEnd();
    if (length != 0) {
      throw "invalid length";
    }
    total_bytes += data.size();
  }
  // state.SetBytesProcessed(total_bytes);
}
BENCHMARK(BM_Thrift_ParseAllTypes_Empty);

void BM_Thrift_ParseAllTypes(benchmark::State& state) {
  string data = GetThriftAllTypes();
  int64_t total_bytes = 0;
  while (state.KeepRunning()) {
    thrift::TestAllTypes message;
    boost::shared_ptr<TMemoryBuffer> buffer(new TMemoryBuffer(
          reinterpret_cast<uint8_t*>(const_cast<char*>(data.data())), data.length()));
    boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(buffer));
    message.read(protocol.get());
    uint32_t length = buffer->readEnd();
    if (length != data.size()) {
      throw "length and data.size() doesn't match.";
    }
    total_bytes += data.size();
  }
  state.SetBytesProcessed(total_bytes);
}
BENCHMARK(BM_Thrift_ParseAllTypes);

}  // namespace
}  // namespace benchmarks

int main(int argc, const char* argv[]) {
  benchmarks::testdata::DataSet dataset;
  if (dataset.ParseFromIstream(&std::cin)) {
    if (dataset.protobuf_data().empty() || dataset.thrift_data().empty()) {
      std::cerr << "Invalid dataset." << std::endl;
      return -1;
    }
    benchmarks::InitializeData(std::move(dataset));
  } else {
    std::cerr << "Cannot read dataset from stdin." << std::endl;
    return -1;
  }

  benchmark::Initialize(&argc, argv);

  benchmarks::PrintInformation();
  
  benchmark::RunSpecifiedBenchmarks();
}

