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
// Authors: Nils Wentzell

/**
 * @file
 * @brief Provides functions to read/write std::array objects from/to HDF5.
 */

#ifndef LIBH5_STL_ARRAY_HPP
#define LIBH5_STL_ARRAY_HPP

#include "../array_interface.hpp"
#include "../complex.hpp"
#include "../macros.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <type_traits>

namespace h5 {

  /**
   * @addtogroup rw_array
   * @{
   */

  /**
   * @brief Write a std::array to an HDF5 dataset/subgroup.
   *
   * @tparam T Value type of the std::array.
   * @tparam N Size of the std::array.
   * @param g h5::group in which the dataset/subgroup is created.
   * @param name Name of the dataset/subgroup to which the std::array is written.
   * @param a std::array to be written.
   */
  template <typename T, size_t N>
  void h5_write(group g, std::string const &name, std::array<T, N> const &a) {
    if constexpr (std::is_same_v<T, std::string>) {
      // array of strings
      auto char_arr = std::array<const char *, N>{};
      std::transform(cbegin(a), cend(a), begin(char_arr), [](std::string const &s) { return s.c_str(); });
      h5_write(g, name, char_arr);
    } else if constexpr (std::is_arithmetic_v<T> or is_complex_v<T> or std::is_same_v<T, dcplx_t> or std::is_same_v<T, char *>
                         or std::is_same_v<T, const char *>) {
      // array of arithmetic/complex types or char* or const char*
      h5::array_interface::array_view v{hdf5_type<T>(), (void *)a.data(), 1, is_complex_v<T>};
      v.slab.count[0]   = N;
      v.slab.stride[0]  = 1;
      v.parent_shape[0] = N;
      h5::array_interface::write(g, name, v, true);
    } else {
      // array of generic type
      auto g2 = g.create_group(name);
      h5_write(g2, "shape", std::array<long, 1>{N});
      for (int i = 0; i < N; ++i) h5_write(g2, std::to_string(i), a[i]);
    }
  }

  /**
   * @brief Read a std::array from an HDF5 dataset/subgroup.
   *
   * @tparam T Value type of the std::array.
   * @tparam N Size of the std::array.
   * @param g h5::group containing the dataset/subgroup.
   * @param name Name of the dataset/subgroup from which the std::array is read.
   * @param a std::array to read into.
   */
  template <typename T, size_t N>
  void h5_read(group g, std::string name, std::array<T, N> &a) {
    if constexpr (std::is_same_v<T, std::string>) {
      // array of strings
      auto char_arr = std::array<char *, N>{};
      h5_read(g, name, char_arr);
      std::copy(cbegin(char_arr), cend(char_arr), begin(a));
      std::for_each(begin(char_arr), end(char_arr), [](char *cb) { free(cb); }); // NOLINT (we have to free the memory allocated by h5_read)
    } else if constexpr (std::is_arithmetic_v<T> or is_complex_v<T> or std::is_same_v<T, dcplx_t> or std::is_same_v<T, char *>
                         or std::is_same_v<T, const char *>) {
      // array of arithmetic/complex types or char* or const char*
      auto ds_info = array_interface::get_dataset_info(g, name);
      H5_EXPECTS(ds_info.rank() == 1 + ds_info.has_complex_attribute);
      H5_EXPECTS(N == ds_info.lengths[0]);

      if constexpr (is_complex_v<T>) {
        // read complex values stored as a compound HDF5 datatype
        if (hdf5_type_equal(ds_info.ty, hdf5_type<dcplx_t>())) {
          h5_read(g, name, reinterpret_cast<std::array<dcplx_t, N> &>(a)); // NOLINT (reinterpret_cast is safe here)
          return;
        }

        // read non-complex data into std::array<std::complex>
        if (!ds_info.has_complex_attribute) {
          std::cerr << "WARNING: HDF5 type mismatch while reading into a std::array: std::complex<" + get_name_of_h5_type(hdf5_type<T>())
                + "> != " + get_name_of_h5_type(ds_info.ty) + "\n";
          std::array<double, N> tmp{};
          h5_read(g, name, tmp);
          std::copy(begin(tmp), end(tmp), begin(a));
          return;
        }
      }

      // use array_interface to read
      array_interface::array_view v{hdf5_type<T>(), (void *)(a.data()), 1, is_complex_v<T>};
      v.slab.count[0]   = N;
      v.slab.stride[0]  = 1;
      v.parent_shape[0] = N;
      array_interface::read(g, name, v, ds_info);
    } else {
      // array of generic type
      auto g2 = g.open_group(name);

      // check that shapes are compatible
      auto h5_shape = std::array<long, 1>{};
      h5_read(g2, "shape", h5_shape);
      H5_EXPECTS(N == h5_shape[0]);

      // read using specialized h5_read implementation
      for (int i = 0; i < N; ++i) h5_read(g2, std::to_string(i), a[i]);
    }
  }

  /** @} */

} // namespace h5

#endif // LIBH5_STL_ARRAY_HPP
