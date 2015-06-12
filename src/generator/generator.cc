#include <iostream>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <random>

#include "proto/message.pb.h"
#include "thrift/message_types.h"
#include "generator/config.pb.h"
#include "generator/testdata.pb.h"
#include "google/protobuf/text_format.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/transport/TBufferTransports.h"

using std::string;

namespace benchmarks {
namespace generator {

using apache::thrift::transport::TMemoryBuffer;
using apache::thrift::protocol::TBinaryProtocol;

namespace {
template<typename T>
string ThriftMessageToString(const T& message) {
  boost::shared_ptr<TMemoryBuffer> buffer(new TMemoryBuffer());
  boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(buffer));
  message.write(protocol.get());
  uint32_t length = buffer->writeEnd();
  return buffer->readAsString(length);
}
}  // namespace

class DataGenerator {
 public:
  virtual ~DataGenerator() {}

  virtual string Generate() = 0;
};

class AbstractDataGenerator : public DataGenerator {
 public:
  AbstractDataGenerator(const GeneratorConfig& config)
      : config_(config),
        bool_generator(0, 1),
        char_generator('a', 'z'),
        int32_generator(),
        int64_generator() {
  }

 protected:
  bool GenerateBool() {
    return bool_generator(random_engine) != 0;
  }

  char GenerateChar() {
    return static_cast<char>(char_generator(random_engine));
  }

  string GenerateString() {
    string result;
    for (int i = 0; i < config_.string_field_size(); ++i) {
      result += GenerateChar();
    }
    return result;
  }

  int32_t GenerateInt32() {
    return int32_generator(random_engine);
  }

  int64_t GenerateInt64() {
    return int64_generator(random_engine);
  }

  const GeneratorConfig& config_;
  std::default_random_engine random_engine;
  std::uniform_int_distribution<int> bool_generator;
  std::uniform_int_distribution<int> char_generator;
  std::uniform_int_distribution<int32_t> int32_generator;
  std::uniform_int_distribution<int64_t> int64_generator;
};

class ProtobufDataGenerator : public AbstractDataGenerator {
 public:
  ProtobufDataGenerator(const GeneratorConfig& config)
      : AbstractDataGenerator(config) {
  }

  string Generate() final {
    proto::TestAllTypes message;
    FillMessage(config_.message_depth(), &message);
    return message.SerializeAsString();
  }

 private:
  void FillMessage(int depth, proto::TestAllTypes* message) {
    FillMessage(message);
    if (depth == 0) {
      return;
    }
    for (int i = 1; i <= config_.children_size(); ++i) {
      FillMessage(depth - 1, message->add_children());
    }
  }

  void FillMessage(proto::TestAllTypes* message) {
    message->set_bool_value(GenerateBool());
    message->set_int32_value(GenerateInt32());
    message->set_int64_value(GenerateInt64());
    message->set_string_value(GenerateString());
    message->set_bytes_value(GenerateString());

    for (int i = 0; i < config_.repeated_field_size(); ++i) {
      message->add_repeated_bool_value(GenerateBool());
      message->add_repeated_int32_value(GenerateInt32());
      message->add_repeated_int64_value(GenerateInt64());
      message->add_repeated_string_value(GenerateString());
      message->add_repeated_bytes_value(GenerateString());
    }
  }
};

class ThriftDataGenerator : public AbstractDataGenerator {
 public:
  ThriftDataGenerator(const GeneratorConfig& config)
      : AbstractDataGenerator(config) {
  }

  string Generate() final {
    thrift::TestAllTypes message;
    FillMessage(config_.message_depth(), &message);
    return ThriftMessageToString(message);
  }

 private:
  void FillMessage(int depth, thrift::TestAllTypes* message) {
    FillMessage(message);
    if (depth == 0) {
      return;
    }
    for (int i = 1; i <= config_.children_size(); ++i) {
      thrift::TestAllTypes child;
      FillMessage(depth - 1, &child);
      message->children.push_back(std::move(child));
    }
  }

  void FillMessage(thrift::TestAllTypes* message) {
    message->bool_value = GenerateBool();
    message->int32_value = GenerateInt32();
    message->int64_value = GenerateInt64();
    message->string_value = GenerateString();
    message->bytes_value = GenerateString();

    for (int i = 0; i < config_.repeated_field_size(); ++i) {
      message->repeated_bool_value.push_back(GenerateBool());
      message->repeated_int32_value.push_back(GenerateInt32());
      message->repeated_int64_value.push_back(GenerateInt64());
      message->repeated_string_value.push_back(GenerateString());
      message->repeated_bytes_value.push_back(GenerateString());
    }
  }
};

}  // namespace generator
}  // namespace benchmarks

using benchmarks::generator::GeneratorConfig;
using benchmarks::generator::DataGenerator;
using benchmarks::generator::ProtobufDataGenerator;
using benchmarks::generator::ThriftDataGenerator;
using benchmarks::testdata::DataSet;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " config_file.txt output.dat" << std::endl;
    return -1;
  }

  GeneratorConfig config;
  std::ifstream fin(argv[1], std::ios::binary);
  if (!fin.is_open()) {
    std::cerr << "Failed to open config file: " << argv[1] << std::endl;
    return -1;
  }
  google::protobuf::io::IstreamInputStream input_stream(&fin);
  if (!google::protobuf::TextFormat::Parse(&input_stream, &config)) {
    std::cerr << "Failed to parse config file: " << argv[1] << std::endl;
    return -1;
  }

  DataSet dataset;
  dataset.mutable_config()->CopyFrom(config);
  dataset.set_protobuf_data(ProtobufDataGenerator(config).Generate());
  dataset.set_thrift_data(ThriftDataGenerator(config).Generate());

  std::ofstream fout(argv[2], std::ios::binary);
  if (!fout.is_open()) {
    std::cerr << "Cannot write output file: " << argv[2] << std::endl;
    return -1;
  }
  dataset.SerializeToOstream(&fout);

  return 0;
}
