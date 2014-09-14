#!/bin/bash

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$script_dir"
export LD_LIBRARY_PATH="$(pwd)/../src/common:$LD_LIBRARY_PATH"
cd cppunit
./testall || exit 1
