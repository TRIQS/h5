// Copyright (c) 2026 Simons Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0.txt
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors: Nils Wentzell

#include <gtest/gtest.h>
#include <h5/h5.hpp>
#include <h5/array_interface.hpp>

#include <array>
#include <complex>
#include <cstddef>
#include <numeric>
#include <string>
#include <type_traits>
#include <vector>

namespace {

  using dcomplex = std::complex<double>;

  // Read a dataset of any rank into a flat row-major std::vector via the
  // array_interface. The destination is a rank-1 view of total size; the
  // file's rank only affects how the bytes were laid out on disk.
  template <typename T>
  std::vector<T> read_flat(h5::group g, std::string const &name) {
    constexpr bool is_cplx = h5::is_complex_v<T>;
    using elem_ty          = std::conditional_t<is_cplx, double, T>;
    auto info              = h5::array_interface::get_dataset_info(g, name);
    auto total             = std::accumulate(info.lengths.begin(), info.lengths.end(), std::size_t{1}, std::multiplies<>());
    std::vector<T> buf(total);
    h5::array_interface::array_view v{h5::hdf5_type<elem_ty>(), buf.data(), 1, is_cplx};
    v.slab.count[0]   = total;
    v.parent_shape[0] = total;
    h5::array_interface::read(g, name, v);
    return buf;
  }

  void check_external_complex_archive(std::string const &filename) {
    h5::file file(filename, 'r');
    auto g = h5::group(file).open_group("group");

    // scalar
    dcomplex scalar;
    h5_read(g, "scalar_complex", scalar);
    EXPECT_EQ(scalar, dcomplex(1.0, 2.0));

    // 1D vector via std::vector<std::complex<double>>
    std::vector<dcomplex> v;
    h5_read(g, "vector_complex", v);
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], dcomplex(1.0, 2.0));
    EXPECT_EQ(v[1], dcomplex(3.0, 4.0));
    EXPECT_EQ(v[2], dcomplex(5.0, 6.0));

    // 1D vector via std::array<std::complex<double>, 3>
    std::array<dcomplex, 3> a{};
    h5_read(g, "vector_complex", a);
    EXPECT_EQ(a[0], dcomplex(1.0, 2.0));
    EXPECT_EQ(a[1], dcomplex(3.0, 4.0));
    EXPECT_EQ(a[2], dcomplex(5.0, 6.0));

    // 2D matrix via the generic array_interface. Both reference files store a (2, 3) row-major
    // matrix with v[r, c] = r + 10*c (and v == conj for the complex variant). See
    // gen_julia_complex.jl for why Julia's matrix is permutedims'd before writing.
    std::vector<dcomplex> const expected_cplx{{0, 0}, {10, 10}, {20, 20}, {1, 1}, {11, 11}, {21, 21}};
    EXPECT_EQ(read_flat<dcomplex>(g, "matrix_complex"), expected_cplx);

    std::vector<double> const expected_flt{0, 10, 20, 1, 11, 21};
    EXPECT_EQ(read_flat<double>(g, "dataset_float"), expected_flt);
  }

} // namespace

TEST(H5, ComplexFromJulia) { check_external_complex_archive("julia_complex.ref.h5"); }

TEST(H5, ComplexFromH5py) { check_external_complex_archive("h5py_complex.ref.h5"); }
