/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2018 by N. Wentzell
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "./test_common.hpp"

#include <h5/h5.hpp>
#include <complex>

#include <hdf5.h>

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
