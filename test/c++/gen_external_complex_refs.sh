#!/usr/bin/env bash
# Regenerate the Julia- and h5py-written complex reference archives used by
# h5_complex_external (C++) and external_complex.py (Python) tests.
#
# Run by hand from this directory whenever the schema changes; the resulting
# .ref.h5 files are checked in. Not invoked by CTest.
set -euo pipefail
cd "$(dirname "$0")"

if command -v module >/dev/null 2>&1; then
  module load julia
fi
julia gen_julia_complex.jl
python3 gen_h5py_complex.py

cp -f julia_complex.ref.h5 h5py_complex.ref.h5 ../python/
echo "Regenerated julia_complex.ref.h5 and h5py_complex.ref.h5 in test/c++ and test/python."
