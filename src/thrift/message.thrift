namespace cpp benchmarks.thrift

struct TestAllTypes {
  1: bool bool_value,
  2: i32 int32_value,
  3: i64 int64_value,
  4: string string_value,
  5: binary bytes_value,
  6: TestAllTypes & message_value,
}

service Calculator {
  i32 Ping(1:TestAllTypes message),
}
