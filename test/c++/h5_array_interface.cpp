// Copyright (c) 2020-2022 Simons Foundation
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
// Authors: Thomas Hahn

#include <gtest/gtest.h>
#include <h5/h5.hpp>

#include <numeric>
#include <vector>

// not working and probably should not work
// TEST(H5, GetLtotAndStridesH5) {
//     // test the get_L_tot_and_strides_h5 function for a 10x10 array with every 2nd element in the view
//     h5::v_t arr_shape{10, 10};
//     h5::v_t view_shape{5, 5};
//     long view_size = 25;
//     std::vector<long> np_strides{20, 2};
//     h5::v_t h5_strides{2, 2};

//     auto [L_tot, strides] = h5::array_interface::get_L_tot_and_strides_h5(np_strides.data(), 2, view_size);
//     EXPECT_EQ(L_tot, arr_shape);
//     EXPECT_EQ(strides, h5_strides);
// }

// not working, should definitely work
// TEST(H5, GetLtotAndStridesH5ContiguousView) {
//     // test the get_L_tot_and_strides_h5 function for a 10x10 array with every element in the view
//     h5::v_t arr_shape{10, 10};
//     h5::v_t view_shape{10, 10};
//     long view_size = 100;
//     std::vector<long> np_strides{10, 1};
//     h5::v_t h5_strides{1, 1};

//     auto [L_tot, strides] = h5::array_interface::get_L_tot_and_strides_h5(np_strides.data(), 2, view_size);
//     EXPECT_EQ(L_tot, arr_shape);
//     EXPECT_EQ(strides, h5_strides);
// }

TEST(H5, ArrayInterface2DArray) {
  // write a 5x5 array and read a 5x3 array (every other column)
  std::vector<int> data(25, 0);
  std::iota(data.begin(), data.end(), 0);

  // create an h5_array_view of rank 2 with dimensions 5x5 of the original data
  int rank   = 2;
  int rows_w = 5;
  int cols_w = 5;
  h5::array_interface::h5_array_view view(h5::hdf5_type<int>(), (void *)data.data(), rank, false);
  view.slab.count[0] = rows_w;
  view.slab.count[1] = cols_w;
  view.L_tot[0]      = rows_w;
  view.L_tot[1]      = cols_w;

  // create file in read/write mode
  h5::file file("2d_array.h5", 'w');

  // write h5_array_view to file
  h5::array_interface::write(file, "view", view, false);

  // reserve memory for reading
  std::vector<int> data_in(15, 0);

  // create an h5_array_view or rank 2 with dimensions 5x3 of the read memory
  int rows_in = 5;
  int cols_in = 3;
  h5::array_interface::h5_array_view view_in(h5::hdf5_type<int>(), (void *)data_in.data(), rank, false);
  view_in.slab.count[0] = rows_in;
  view_in.slab.count[1] = cols_in;
  view_in.L_tot[0]      = rows_in;
  view_in.L_tot[1]      = cols_in;

  // create an hyperslab to select the data to be read from the file (every other column -> stride in second dimension is 2)
  h5::array_interface::hyperslab slab_in(rank, false);
  slab_in.count[0]  = rows_in;
  slab_in.count[1]  = cols_in;
  slab_in.stride[0] = 1;
  slab_in.stride[1] = 2;

  // get h5_lengths_type from the dataset in the file
  auto lengths_type = h5::array_interface::get_h5_lengths_type(file, "view");

  // read data from file
  h5::array_interface::read(file, "view", view_in, lengths_type, slab_in);

  // check results
  for (int i = 0; i < rows_in; ++i) {
    for (int j = 0; j < cols_in; ++j) { EXPECT_EQ(data_in[i * cols_in + j], data[i * cols_w + j * 2]); }
  }
}
