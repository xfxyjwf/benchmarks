namespace cpp benchmarks.thrift

struct TestAllTypes {
  1: bool bool_value,
  2: i32 int32_value,
  3: i64 int64_value,
  4: string string_value,
  5: binary bytes_value,

  101: list<bool> repeated_bool_value,
  102: list<i32> repeated_int32_value,
  103: list<i64> repeated_int64_value,
  104: list<string> repeated_string_value,
  105: list<binary> repeated_bytes_value,

  201: list<TestAllTypes> children,
}
