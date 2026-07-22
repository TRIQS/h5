#!/usr/bin/env bash
# Regenerate the cross-language DictNonStrKey reference archives:
#   dict_tuplekey.ref.h5  - written by the Python h5 module, read by C++    (h5_map.cpp: MapWithNonStrKeyFromPython)
#   map_pairkey.ref.h5    - written by C++ (std::map<pair,...> and <int,...>), read by Python (archive.py: test_dict_*_from_cpp)
#
# Run by hand after building whenever the layout changes; the resulting .ref.h5
# files are checked in. Not invoked by CTest. Pass the build dir as $1 (default: ../../build).
set -euo pipefail
cd "$(dirname "$0")"
BUILD=$(cd "${1:-../../build}" && pwd)

# Python-written archive via the h5 module (leaves dict_tuplekey.ref.h5 in this directory).
PYTHONPATH="$BUILD/python:${PYTHONPATH:-}" python3 gen_dict_tuplekey.py

# C++-written archive: the h5_map test writes test_map_pairkey.h5 deterministically.
"$BUILD/test/c++/h5_map" --gtest_filter='H5.MapWithNonStrKeyType' >/dev/null
cp -f "$BUILD/test/c++/test_map_pairkey.h5" ../python/map_pairkey.ref.h5

echo "Regenerated dict_tuplekey.ref.h5 (test/c++) and map_pairkey.ref.h5 (test/python)."
