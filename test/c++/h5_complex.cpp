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

#include "./test_common.hpp"

#include <h5/h5.hpp>
#include <complex>

// clang-format off
TEST(H5, ComplexBkwd){

  {
    h5::file file("complex_old.h5", 'w');
    h5::group top(file);

    auto g = top.create_group("cplx");

    double r = 1.0, i = 2.0;
    h5_write(g, "r", r);
    h5_write(g, "i", i);
  }

  {
    h5::file file("complex_old.h5", 'r');

    std::complex<double> c;
    h5_read(file, "cplx", c);

    dcomplex exact = {1.0, 2.0};
    EXPECT_EQ(c, exact);
  }
};

TEST(H5, ComplexCompound){

  std::array<h5::dcplx_t, 4> arr = { h5::dcplx_t{0.0, 0.0}, h5::dcplx_t{0.0, 1.0}, h5::dcplx_t{1.0, 0.0}, h5::dcplx_t{1.0, 1.0} };
  auto scal = h5::dcplx_t{2.0, 2.0};

  {
    h5::file file("complex_compound.h5", 'w');

    h5_write(file, "cplx_arr", arr);
    h5_write(file, "cplx_scal", scal);
  }

  {
    h5::file file("complex_compound.h5", 'r');

    std::array<std::complex<double>, 4> arr_in;
    std::complex<double> scal_in;

    h5_read(file, "cplx_arr", arr_in);
    h5_read(file, "cplx_scal", scal_in);

    for(int i = 0; i < arr.size(); ++i){
      EXPECT_EQ(arr_in[i].real(), arr[i].r);
      EXPECT_EQ(arr_in[i].imag(), arr[i].i);
    }

    EXPECT_EQ(scal_in.real(), scal.r);
    EXPECT_EQ(scal_in.imag(), scal.i);
  }

};

// clang-format on
