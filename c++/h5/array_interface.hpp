// Copyright (c) 2019-2020 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides a generic interface to read/write n-dimensional arrays from/to HDF5.
 */

#ifndef LIBH5_ARRAY_INTERFACE_HPP
#define LIBH5_ARRAY_INTERFACE_HPP

#include "./group.hpp"
#include "./object.hpp"

#include <algorithm>
#include <numeric>
#include <string>
#include <utility>

namespace h5::array_interface {

  /**
   * @addtogroup rw_arrayinterface
   * @{
   */

  /// Simple struct to store basic information about an HDF5 dataset.
  struct dataset_info {
    /// Shape of the dataspace in the dataset.
    v_t lengths;

    /// h5::datatype stored in the dataset.
    datatype ty;

    /// Whether the stored values are complex.
    bool has_complex_attribute;

    /// Get the rank of the dataspace in the dataset.
    [[nodiscard]] int rank() const { return static_cast<int>(lengths.size()); }
  };

  /**
   * @brief Struct representing an HDF5 hyperslab.
   *
   * @details A hyperslab is used to select elements from an n-dimensional array/dataspace and it is defined
   * by 4 arrays of the same size as the rank of the underlying dataspace
   * (see <a href="https://docs.hdfgroup.org/hdf5/v1_12/group___h5_s.html">HDF5 docs</a>):
   * - `offset`: Origin of the hyperslab in the dataspace.
   * - `stride`: The number of elements to skip between each element or block to be selected. If the stride
   * parameter is set to `NULL`, the stride size defaults to 1 in each dimension.
   * - `count`: The number of elements or blocks to select along each dimension.
   * - `block`: The size of a block selected from the dataspace. If the block parameter is set to `NULL`,
   * the block size defaults to a single element in each dimension, as if the block array was set to all ones.
   *
   * The imaginary part of a complex array/dataspace is treated as just another dimension, i.e. its rank is
   * increased by one.
   *
   * The following example selects every second column in a `7x7` dataspace:
   *
   * @code
   * h5::array_interface::hyperslab slab;
   * slab.offset = {0, 0};
   * slab.stride = {1, 2};
   * slab.count = {7, 4};
   * slab.block = {1, 1};
   * @endcode
   *
   * For a complex valued dataspace, the same example would look like this:
   *
   * @code
   * h5::array_interface::hyperslab slab;
   * slab.offset = {0, 0, 0};
   * slab.stride = {1, 2, 1};
   * slab.count = {7, 4, 2};
   * slab.block = {1, 1, 1};
   * @endcode
   */
  struct hyperslab {
    /// Index offset for each dimension.
    v_t offset;

    /// Stride in each dimension (in the HDF5 sense).
    v_t stride;

    /// Number of elements or blocks to select along each dimension.
    v_t count;

    /// Shape of a single block selected from the dataspace.
    v_t block;

    /**
     * @brief Construct a new empty hyperslab for a dataspace of a given rank.
     *
     * @details A complex hyperslab has an additional dimension for the imaginary part. By default, `offset` and
     * `count` are set to zero and `stride` and `block` are set to one. If complex valued, `count.back() = 2`.
     *
     * @param rank Rank of the underlying dataspace (excluding the possible added imaginary dimension).
     * @param is_complex Whether the data is complex valued.
     */
    hyperslab(int rank, bool is_complex)
       : offset(rank + is_complex, 0), stride(rank + is_complex, 1), count(rank + is_complex, 0), block(rank + is_complex, 1) {
      if (is_complex) {
        stride[rank] = 1;
        count[rank]  = 2;
      }
    }

    /// Default constructor leaves the hyperslab empty (uninitialized).
    hyperslab() = default;

    /// Get the rank of the hyperslab (including the possible added imaginary dimension).
    [[nodiscard]] int rank() const { return static_cast<int>(count.size()); }

    /// Check whether the hyperslab is empty (has been initialized).
    [[nodiscard]] bool empty() const { return count.empty(); }

    /// Get the shape of the selected hyperslab.
    [[nodiscard]] v_t shape() const {
      v_t shape(rank());
      std::transform(count.begin(), count.end(), block.begin(), shape.begin(), std::multiplies<>());
      return shape;
    }

    /// Get the total number of elements in the hyperslab.
    [[nodiscard]] auto size() const {
      auto sh = shape();
      return std::accumulate(sh.begin(), sh.end(), (hsize_t)1, std::multiplies<>());
    }
  };

  /**
   * @brief Struct representing a view on an n-dimensional array/dataspace.
   *
   * @details A view consists of the parent array and of an h5::array_interface::hyperslab specifying a selection.
   * The parent array is defined by a pointer to its data and its shape.
   *
   * Note that the shape of the parent array does not necessarily have to correspond to the actual shape and size of
   * the underlying memory. It is only used to select the correct elements in the hyperslab.
   *
   * If the data of the array is complex, its imaginary part is treated as just another dimension.
   */
  struct array_view {
    /// h5::datatype stored in the array.
    datatype ty;

    /// Pointer to the data of the array.
    void *start;

    /// Shape of the (contiguous) parent array.
    v_t parent_shape;

    /// h5::array_interface::hyperslab specifying the selection of the view.
    hyperslab slab;

    /// Whether the data is complex valued.
    bool is_complex;

    /**
     * @brief Construct a new empty array view.
     *
     * @details A complex view has an additional dimension for the imaginary part. The shape of the
     * parent array is left uninitialized and the h5::array_interface::hyperslab is empty.
     *
     * @param ty h5::datatype of the array.
     * @param start Pointer to the data of the parent array.
     * @param rank Rank of the parent array (excluding the possible added imaginary dimension).
     * @param is_complex Whether the data is complex valued.
     */
    array_view(datatype ty, void *start, int rank, bool is_complex)
       : ty(std::move(ty)), start(start), parent_shape(rank + is_complex), slab(rank, is_complex), is_complex(is_complex) {
      if (is_complex) parent_shape[rank] = 2;
    }

    /// Get the rank of the view (including the possible added imaginary dimension).
    [[nodiscard]] int rank() const { return slab.rank(); }
  };

  /**
   * @brief Given a view on an n-dimensional array (dataspace) by specifying its numpy/nda-style strides and
   * its size (number of elements in the view), calculate the shape of a possible parent array and the corresponding
   * HDF5 strides of the view.
   *
   * @details The memory layout is assumend to be in C-order. Suppose `parent_shape` is an array containing the shape of
   * the n-dimensional parent array, `np_strides` contains the numpy strides and `h5_strides` are the HDF5 strides. Then
   * the following equations have to hold (rank = N):
   * - `np_strides[N - 1] = h5_strides[N - 1]`
   * - `np_strides[N - 2] = h5_strides[N - 2] * parent_shape[N - 1]`
   * - `np_strides[N - 3] = h5_strides[N - 3] * parent_shape[N - 1] * parent_shape[N - 2]`
   * - `...`
   * - `np_strides[N - N] = h5_strides[N - N] * parent_shape[N - 1] * parent_shape[N - 2] * ... * parent_shape[1]`.
   *
   * Note that `np_strides[N - i] = m * s[N - 1] * s[N - 2] * ... * s[N - i + 1]`, where `0 < m < s[N - i]` is an integer
   * an `s` is the true shape of the parent array.
   *
   * @note The shape of the parent array as well as the HDF5 strides that fulfill the above equations are not unique.
   * This is not a problem as long as HDF5 manages to select the correct elements in memory.
   *
   * @param np_strides Numpy/nda-style strides.
   * @param rank Rank of the n-dimensional parent array.
   * @param view_size Number of elements in the given view.
   * @return std::pair containing the shape of the parent array and the HDF5 strides of the view.
   */
  std::pair<v_t, v_t> get_parent_shape_and_h5_strides(long const *np_strides, int rank, long view_size);

  /**
   * @brief Retrieve the shape and the h5::datatype from a dataset.
   *
   * @param ds h5::dataset.
   * @return h5::array_interface::dataset_info containing the shape and HDF5 type of the dataset.
   */
  dataset_info get_dataset_info(dataset ds);

  /**
   * @brief Retrieve the shape and the h5::datatype from a dataset with a given name in the given group.
   *
   * @param g h5::group containing the dataset.
   * @param name Name of the dataset.
   * @return h5::array_interface::dataset_info containing the shape and HDF5 type of the dataset.
   */
  dataset_info get_dataset_info(group g, std::string const &name);

  /**
   * @brief Write an array view to an HDF5 dataset.
   *
   * @details If a link with the given name already exists, it is first unlinked.
   *
   * @param g h5::group in which the dataset is created.
   * @param name Name of the dataset
   * @param v h5::array_interface::array_view to be written.
   * @param compress Whether to compress the dataset.
   */
  void write(group g, std::string const &name, array_view const &v, bool compress);

  /**
   * @brief Write an array view to a selected hyperslab of an existing HDF5 dataset.
   *
   * @details It checks if the number of elements in the view is the same as selected in the hyperslab and if the
   * datatypes are compatible. Otherwise, an exception is thrown.
   *
   * @param g h5::group which contains the dataset.
   * @param name Name of the dataset.
   * @param v h5::array_interface::array_view to be written.
   * @param sl h5::array_interface::hyperslab specifying the selection to be written to.
   */
  void write_slice(group g, std::string const &name, array_view const &v, hyperslab sl);

  /**
   * @brief Write an array view to an HDF5 attribute.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param v v h5::array_interface::array_view to be written.
   */
  void write_attribute(object obj, std::string const &name, array_view v);

  /**
   * @brief Read a given hyperslab from an HDF5 dataset into an array view.
   *
   * @param g h5::group which contains the dataset.
   * @param name Name of the dataset.
   * @param v h5::array_interface::array_view to read into.
   * @param sl h5::array_interface::hyperslab specifying the selection to read from.
   */
  void read(group g, std::string const &name, array_view v, hyperslab sl = {});

  /**
   * @brief Read from an HDF5 attribute into an array view.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param v h5::array_interface::array_view to read into.
   */
  void read_attribute(object obj, std::string const &name, array_view v);

  /** @} */

} // namespace h5::array_interface

#endif // LIBH5_ARRAY_INTERFACE_HPP
