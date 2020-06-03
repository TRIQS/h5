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

  // Create the compound datatype for memory.
  h5::datatype mem_dt = H5Tcreate(H5T_COMPOUND, sizeof(h5::cplx_t));
  H5Tinsert(mem_dt, "r", HOFFSET(h5::cplx_t, r), H5T_NATIVE_DOUBLE);
  H5Tinsert(mem_dt, "i", HOFFSET(h5::cplx_t, i), H5T_NATIVE_DOUBLE);

  // Create the compound datatype for the file.
  h5::datatype file_dt = H5Tcreate(H5T_COMPOUND, 16);
  H5Tinsert(file_dt, "r", 0, H5T_IEEE_F64LE);
  H5Tinsert(file_dt, "i", 8, H5T_IEEE_F64LE);

  std::array<hsize_t, 1> dims = {4};
  std::array<h5::cplx_t, 4> arr = { h5::cplx_t{0.0, 0.0}, h5::cplx_t{0.0, 1.0}, h5::cplx_t{1.0, 0.0}, h5::cplx_t{1.0, 1.0} };

  {  // Write array

    h5::file file("complex_compound.h5", 'w');

    h5::dataspace dspace = H5Screate_simple(1, dims.data(), nullptr);
    h5::dataset ds = H5Dcreate(file, "cplx_arr", file_dt, dspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(ds, mem_dt, H5S_ALL, H5S_ALL, H5P_DEFAULT, arr.data());

  }

  auto scal = h5::cplx_t{2.0, 2.0};

  { // Write Scalar
    
    h5::file file("complex_compound.h5", 'a');

    h5::dataspace dspace = H5Screate(H5S_SCALAR);
    h5::dataset ds = H5Dcreate(file, "cplx_scal", file_dt, dspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(ds, mem_dt, H5S_ALL, H5S_ALL, H5P_DEFAULT, &scal);
  }

  {
    h5::file file("complex_compound.h5", 'r');

    std::array<std::complex<double>, 4> arr_in;
    std::complex<double> scal_in;

    h5_read(file, "cplx_arr", arr_in);
    h5_read(file, "cplx_scal", scal_in);

    //h5::dataset ds = H5Dopen(file, "cplx", H5P_DEFAULT);
    //h5::dataspace dspace = H5Dget_space(ds);
    //H5Dread(ds, mem_dt, H5S_ALL, H5S_ALL, H5P_DEFAULT, arr_in.data());

    for(int i = 0; i < dims[0]; ++i){
      EXPECT_EQ(arr_in[i].real(), arr[i].r);
      EXPECT_EQ(arr_in[i].imag(), arr[i].i);
    }

    EXPECT_EQ(scal_in.real(), scal.r);
    EXPECT_EQ(scal_in.imag(), scal.i);
  }

};

// clang-format on
