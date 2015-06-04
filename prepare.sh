#!/bin/bash

# Remove tests from protobuf because they require gtest to run and we don't
# actually need them in this benchmark.
sed -i "/tests.cmake/d" protobuf/cmake/CMakeLists.txt
