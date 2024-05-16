// Copyright (c) 2024 Simons Foundation
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

#include <hdf5_hl.h>

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
    EXPECT_TRUE(parent_shape[i] >= view_shape[i] * h5_strides[i]);
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
  h5::group group(file);
  std::vector<int> data(100, 0);
  std::iota(data.begin(), data.end(), 0);
  int rank = 1;
  int size = static_cast<int>(data.size());

  // write the full array
  h5::array_interface::array_view view_1(h5::hdf5_type<int>(), (void *)data.data(), rank, false);
  view_1.slab.count[0]   = size;
  view_1.parent_shape[0] = size;
  h5::array_interface::write(file, "view_1", view_1, true);

  // read the full array
  std::vector<int> data_in_1(data.size(), 0);
  h5::array_interface::array_view view_in_1(h5::hdf5_type<int>(), (void *)data_in_1.data(), rank, false);
  view_in_1.slab.count[0]   = size;
  view_in_1.parent_shape[0] = size;
  h5::array_interface::read(file, "view_1", view_in_1);

  // check results
  for (int i = 0; i < size; ++i) { EXPECT_EQ(data_in_1[i], data[i]); }

  // write blocks of size 10, skipping every other block, starting with the second block
  h5::array_interface::array_view view_2(h5::hdf5_type<int>(), (void *)data.data(), rank, false);
  view_2.slab.offset[0]  = 10;
  view_2.slab.stride[0]  = 20;
  view_2.slab.count[0]   = 5;
  view_2.slab.block[0]   = 10;
  view_2.parent_shape[0] = size;
  h5::array_interface::write(file, "view_2", view_2, true);

  // read the data written to view_2 into a 1D array
  std::vector<int> data_in_2(50, 0);
  h5::array_interface::array_view view_in_2(h5::hdf5_type<int>(), (void *)data_in_2.data(), rank, false);
  view_in_2.slab.count[0]   = 50;
  view_in_2.parent_shape[0] = 50;
  h5::array_interface::read(file, "view_2", view_in_2);

  // read blocks of size 10, skipping every other block, starting with the second block directly from view_1
  std::vector<int> data_in_3(50, 0);
  h5::array_interface::array_view view_in_3(h5::hdf5_type<int>(), (void *)data_in_3.data(), rank, false);
  view_in_3.slab.count[0]   = 50;
  view_in_3.parent_shape[0] = 50;
  h5::array_interface::read(file, "view_1", view_in_3, view_2.slab);

  // check results
  for (int i = 0; i < 50; ++i) { EXPECT_EQ(data_in_2[i], data_in_3[i]); }

  // write the blocks selected in view_2 to a 5x10 2D array
  h5::v_t shape_3        = {5, 10};
  h5::dataspace dspace_3 = H5Screate_simple(2, shape_3.data(), nullptr);
  std::ignore            = group.create_dataset("view_3", h5::hdf5_type<int>(), dspace_3);
  h5::array_interface::hyperslab slab_4(2, false);
  slab_4.count = {5, 1};
  slab_4.block = {1, 10};
  h5::array_interface::write_slice(file, "view_3", view_2, slab_4);

  // read the 2D view_3 into a 1D array
  std::vector<int> data_in_4(50, 0);
  h5::array_interface::array_view view_in_4(h5::hdf5_type<int>(), (void *)data_in_4.data(), rank, false);
  view_in_4.slab.count[0]   = 50;
  view_in_4.parent_shape[0] = 50;
  h5::array_interface::read(file, "view_3", view_in_4, slab_4);

  for (int i = 0; i < 50; ++i) { EXPECT_EQ(data_in_4[i], data_in_3[i]); }
}

TEST(H5, ArrayInterface3DArray) {
  // test the array interface for a 3D array
  h5::file file("3d_array.h5", 'w');
  h5::group group(file);
  std::vector<int> data(27, 0);
  std::iota(data.begin(), data.end(), 0);
  int rank = 3;
  int size = static_cast<int>(data.size());

  // write the full data as a 3x3x3 array
  h5::array_interface::array_view view_1(h5::hdf5_type<int>(), (void *)data.data(), rank, false);
  view_1.slab.count   = {3, 3, 3};
  view_1.parent_shape = {3, 3, 3};
  h5::array_interface::write(file, "view_1", view_1, true);

  // read the full array as a 1D array
  std::vector<int> data_in_1(data.size(), 0);
  h5::array_interface::array_view view_in_1(h5::hdf5_type<int>(), (void *)data_in_1.data(), 1, false);
  view_in_1.slab.count[0]   = size;
  view_in_1.parent_shape[0] = size;
  h5::array_interface::read(file, "view_1", view_in_1);

  // check results
  for (int i = 0; i < size; ++i) { EXPECT_EQ(data_in_1[i], data[i]); }

  // write the 3x3 arrays starting at (0, 0, 0) and (2, 0, 0) into a 6x3 array
  h5::array_interface::array_view view_2(h5::hdf5_type<int>(), (void *)data.data(), rank, false);
  view_2.slab.offset     = {0, 0, 0};
  view_2.slab.stride     = {2, 1, 1};
  view_2.slab.count      = {2, 1, 1};
  view_2.slab.block      = {1, 3, 3};
  view_2.parent_shape    = {3, 3, 3};
  h5::v_t shape_2        = {6, 3};
  h5::dataspace dspace_2 = H5Screate_simple(2, shape_2.data(), nullptr);
  std::ignore            = group.create_dataset("view_2", h5::hdf5_type<int>(), dspace_2);
  h5::array_interface::hyperslab slab_2(2, false);
  slab_2.count = shape_2;
  h5::array_interface::write_slice(file, "view_2", view_2, slab_2);

  // read in the data written to view_2 into a 2D array
  std::vector<int> data_in_2(18, 0);
  h5::array_interface::array_view view_in_2(h5::hdf5_type<int>(), (void *)data_in_2.data(), 2, false);
  view_in_2.slab         = slab_2;
  view_in_2.parent_shape = shape_2;
  h5::array_interface::read(file, "view_2", view_in_2);

  // read in the 3x3 arrays starting at (0, 0, 0) and (2, 0, 0) directly from view_1
  std::vector<int> data_in_3(18, 0);
  h5::array_interface::array_view view_in_3(h5::hdf5_type<int>(), (void *)data_in_3.data(), 2, false);
  view_in_3.slab         = slab_2;
  view_in_3.parent_shape = shape_2;
  h5::array_interface::read(file, "view_1", view_in_3, view_2.slab);

  // check results
  for (int i = 0; i < 18; ++i) { EXPECT_EQ(data_in_2[i], data_in_3[i]); }
}
