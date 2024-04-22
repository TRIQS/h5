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

// Print container.
template <typename C>
void print(const C &c) {
  for (const auto &v : c) std::cout << v << " ";
  std::cout << std::endl;
}

// Check the set of equations that relate numpy/nda-style strides to HDF5-style strides.
void check_strides(const std::vector<long> &np_strides, const std::vector<long> &view_shape) {
  // get view size and rank
  auto view_size = std::accumulate(view_shape.begin(), view_shape.end(), 1l, std::multiplies<>());
  int rank       = static_cast<int>(view_shape.size());

  // get parent shape and h5 strides
  auto [parent_shape, h5_strides] = h5::array_interface::get_parent_shape_and_h5_strides(np_strides.data(), rank, view_size);

  // check that the number of strides is the same
  EXPECT_EQ(np_strides.size(), h5_strides.size());

  // check that the number of strides is the same as the rank of the parent shape
  EXPECT_EQ(np_strides.size(), parent_shape.size());

  // check that the strides are consistent with the parent shape
  h5::hsize_t product = 1;
  for (int i = static_cast<int>(np_strides.size()) - 1; i >= 0; --i) {
    EXPECT_EQ(np_strides[i], product * h5_strides[i]);
    product *= parent_shape[i];
    EXPECT_TRUE(parent_shape[i] >= view_shape[i]);
  }
}

TEST(H5, GetParentShapeAndH5Strides1D) {
  std::vector<long> np_strides(1), view_shape(1);

  // 1D array of size 10: contiguous data, all elements
  np_strides = {1};
  view_shape = {10};
  check_strides(np_strides, view_shape);

  // 1D array of size 10: contiguous data, first 5 elements
  view_shape = {5};
  check_strides(np_strides, view_shape);

  // 1D array of size 10: every other element
  np_strides = {2};
  view_shape = {5};
  check_strides(np_strides, view_shape);

  // 1D array of size 10: every third element
  np_strides = {3};
  view_shape = {4};
  check_strides(np_strides, view_shape);

  // 1D array of size 10: every seventh element
  np_strides = {7};
  view_shape = {2};
  check_strides(np_strides, view_shape);
}

TEST(H5, GetParentShapeAndH5Strides2D) {
  std::vector<long> np_strides(2), view_shape(2);

  // 2D array of size 10x10: contiguous data, all elements
  np_strides = {10, 1};
  view_shape = {10, 10};
  check_strides(np_strides, view_shape);

  // 2D array of size 10x10: contiguous data, first 5x5 elements
  view_shape = {5, 5};
  check_strides(np_strides, view_shape);

  // 2D array of size 10x10: every other row and column
  np_strides = {20, 2};
  view_shape = {5, 5};
  check_strides(np_strides, view_shape);

  // 2D array of size 10x10: every third row and the first four columns
  np_strides = {30, 1};
  view_shape = {4, 4};
  check_strides(np_strides, view_shape);
}

TEST(H5, GetParentShapeAndH5Strides3D) {
  std::vector<long> np_strides(3), view_shape(3);

  // 3D array of size 10x10x10: contiguous data, all elements
  np_strides = {100, 10, 1};
  view_shape = {10, 10, 10};
  check_strides(np_strides, view_shape);

  // 3D array of size 10x10x10: contiguous data, first 2x3x4 elements
  view_shape = {2, 3, 4};
  check_strides(np_strides, view_shape);

  // 3D array of size 10x10x10: every other element
  np_strides = {200, 20, 2};
  view_shape = {5, 5, 5};
  check_strides(np_strides, view_shape);
}

TEST(H5, ArrayInterface1DArray) {
  // test the array interface for a 1D array
  h5::file file("1d_array.h5", 'w');
  std::vector<int> data(100, 0);
  std::iota(data.begin(), data.end(), 0);
  int rank = 1;
  int size = static_cast<int>(data.size());

  // write the full array
  h5::array_interface::array_view full_view(h5::hdf5_type<int>(), (void *)data.data(), rank, false);
  full_view.slab.count[0]   = size;
  full_view.parent_shape[0] = size;
  h5::array_interface::write(file, "full_view", full_view, true);

  // read the full array
  std::vector<int> full_data_in(data.size(), 0);
  h5::array_interface::array_view full_view_in(h5::hdf5_type<int>(), (void *)full_data_in.data(), rank, false);
  full_view_in.slab.count[0]   = size;
  full_view_in.parent_shape[0] = size;
  auto full_view_ds_info       = h5::array_interface::get_dataset_info(file, "full_view");
  h5::array_interface::read(file, "full_view", full_view_in, full_view_ds_info, full_view.slab);

  // check results
  for (int i = 0; i < size; ++i) { EXPECT_EQ(full_data_in[i], data[i]); }

  // // write blocks of size 10, skipping every other block, starting with the second block
  // h5::array_interface::array_view block_view(h5::hdf5_type<int>(), (void *)data.data(), rank, false);
  // block_view.slab.offset[0]  = 10;
  // block_view.slab.stride[0]  = 20;
  // block_view.slab.count[0]   = 5;
  // block_view.slab.block[0]   = 10;
  // block_view.parent_shape[0] = size;
  // h5::array_interface::write(file, "block_view", block_view, true);

  // // read blocks of size 10, skipping every other block, starting with the second block
  // std::vector<int> block_data_in(50, 0);
  // h5::array_interface::array_view block_view_in(h5::hdf5_type<int>(), (void *)block_data_in.data(), rank, false);
  // block_view_in.slab.count[0]   = 50;
  // block_view_in.parent_shape[0] = 50;
  // h5::array_interface::read(file, "full_view", block_view_in, full_view_lt, block_view_in.slab);

  // read the block view
  // std::vector<int> block_data_in(50, 0);
  // h5::array_interface::array_view block_view_in(h5::hdf5_type<int>(), (void *)block_data_in.data(), rank, false);
  // block_view_in.slab.count[0]   = 50;
  // block_view_in.parent_shape[0] = 50;
  // auto block_view_lt            = h5::array_interface::get_dataset_info(file, "block_view");
  // h5::array_interface::read(file, "block_view", block_view_in, block_view_lt, block_view_in.slab);
}

// TEST(H5, ArrayInterface2DArray) {
//   // write a 5x5 array and read a 5x3 array (every other column)
//   std::vector<int> data(25, 0);
//   std::iota(data.begin(), data.end(), 0);

//   // create an array_view of rank 2 with dimensions 5x5 of the original data
//   int rank   = 2;
//   int rows_w = 5;
//   int cols_w = 5;
//   h5::array_interface::array_view view(h5::hdf5_type<int>(), (void *)data.data(), rank, false);
//   // view.slab.count[0]   = rows_w;
//   // view.slab.count[1]   = cols_w;
//   // view.parent_shape[0] = rows_w;
//   // view.parent_shape[1] = cols_w;
//   view.slab.count[0]   = 1;
//   view.slab.count[1]   = 1;
//   view.slab.block[0]   = 5;
//   view.slab.block[1]   = 5;
//   view.parent_shape[0] = rows_w;
//   view.parent_shape[1] = cols_w;

//   // create file in read/write mode
//   h5::file file("2d_array.h5", 'w');

//   // write array_view to file
//   h5::array_interface::write(file, "view", view, false);

//   // reserve memory for reading
//   std::vector<int> data_in(15, 0);

//   // create an array_view or rank 2 with dimensions 5x3 of the read memory
//   int rows_in = 5;
//   int cols_in = 3;
//   h5::array_interface::array_view view_in(h5::hdf5_type<int>(), (void *)data_in.data(), rank, false);
//   view_in.slab.count[0]   = rows_in;
//   view_in.slab.count[1]   = cols_in;
//   view_in.parent_shape[0] = rows_in;
//   view_in.parent_shape[1] = cols_in;

//   // create an hyperslab to select the data to be read from the file (every other column -> stride in second dimension is 2)
//   h5::array_interface::hyperslab slab_in(rank, false);
//   slab_in.count[0]  = rows_in;
//   slab_in.count[1]  = cols_in;
//   slab_in.stride[0] = 1;
//   slab_in.stride[1] = 2;

//   // get dataset_info from the dataset in the file
//   auto lengths_type = h5::array_interface::get_dataset_info(file, "view");

//   // read data from file
//   h5::array_interface::read(file, "view", view_in, lengths_type, slab_in);

//   // check results
//   for (int i = 0; i < rows_in; ++i) {
//     for (int j = 0; j < cols_in; ++j) { EXPECT_EQ(data_in[i * cols_in + j], data[i * cols_w + j * 2]); }
//   }
// }
