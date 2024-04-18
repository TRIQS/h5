// Copyright (c) 2020 Simons Foundation
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

#include <array>
#include <complex>

TEST(H5, ComplexBackwardCompatibility) {
  // write and read a complex number the old way
  std::complex<double> z{1.0, 2.0};

  {
    h5::file file("complex_old.h5", 'w');
    h5::group top(file);

    auto g = top.create_group("cplx");

    h5_write(g, "r", z.real());
    h5_write(g, "i", z.imag());
  }

  {
    h5::file file("complex_old.h5", 'r');

    std::complex<double> z_in;
    h5_read(file, "cplx", z_in);

    EXPECT_EQ(z, z_in);
  }
};

TEST(H5, ComplexCompoundType) {
  // write an array of h5::dxplx_t and read it into an array of std::complex<double>
  std::array<h5::dcplx_t, 4> arr = {h5::dcplx_t{0.0, 0.0}, h5::dcplx_t{0.0, 1.0}, h5::dcplx_t{1.0, 0.0}, h5::dcplx_t{1.0, 1.0}};
  h5::dcplx_t z{2.0, 2.0};

  {
    h5::file file("complex_compound.h5", 'w');

    h5_write(file, "cplx_arr", arr);
    h5_write(file, "cplx_scal", z);
  }

  {
    h5::file file("complex_compound.h5", 'r');

    std::array<std::complex<double>, 4> arr_in;
    std::complex<double> z_in;

    h5_read(file, "cplx_arr", arr_in);
    h5_read(file, "cplx_scal", z_in);

    for (int i = 0; i < arr.size(); ++i) {
      EXPECT_EQ(arr_in[i].real(), arr[i].r);
      EXPECT_EQ(arr_in[i].imag(), arr[i].i);
    }

    EXPECT_EQ(z_in.real(), z.r);
    EXPECT_EQ(z_in.imag(), z.i);
  }
};
