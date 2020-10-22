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

#ifndef LIBH5_STL_ARRAY_HPP
#define LIBH5_STL_ARRAY_HPP

#include "../array_interface.hpp"

#include <array>
#include <algorithm>
#include <type_traits>

namespace h5 {

  /**
   * Writes std::array into an hdf5 file
   *
   * @tparam T
   * @param g HDF5 group
   * @param name Name of the object in the HDF5 file
   * @param a Array to save in the file
   */
  template <typename T, size_t N>
  void h5_write(group g, std::string const &name, std::array<T, N> const &a) {

    if constexpr (std::is_same_v<T, std::string>) {
      auto char_arr = std::array<const char *, N>{};
      std::transform(cbegin(a), cend(a), begin(char_arr), [](std::string const &s) { return s.c_str(); });
      h5_write(g, name, char_arr);

    } else if constexpr (std::is_arithmetic_v<
                            T> or is_complex_v<T> or std::is_same_v<T, dcplx_t> or std::is_same_v<T, char *> or std::is_same_v<T, const char *>) {

      h5::array_interface::h5_array_view v{hdf5_type<T>(), (void *)a.data(), 1, is_complex_v<T>};
      v.slab.count[0]  = N;
      v.slab.stride[0] = 1;
      v.L_tot[0]       = N;

      h5::array_interface::write(g, name, v, true);

    } else { // generic unknown type to hdf5
      auto g2 = g.create_group(name);
      h5_write(g2, "shape", std::array<long, 1>{N});
      for (int i = 0; i < N; ++i) h5_write(g2, std::to_string(i), a[i]);
    }
  }

  /**
   * Reads std::array from HDF5
   *
   * Use implementation h5_read from the array_interface
   *
   * @tparam T
   * @param g HDF5 group
   * @param name Name of the object in the HDF5 file
   * @param a Array to save from the file
   */
  template <typename T, size_t N>
  void h5_read(group g, std::string name, std::array<T, N> &a) {

    if constexpr (std::is_same_v<T, std::string>) {
      auto char_arr = std::array<char *, N>{};
      h5_read(g, name, char_arr);
      std::copy(cbegin(char_arr), cend(char_arr), begin(a));
      std::for_each(begin(char_arr), end(char_arr), [](char *cb) { free(cb); });

    } else if constexpr (std::is_arithmetic_v<
                            T> or is_complex_v<T> or std::is_same_v<T, dcplx_t> or std::is_same_v<T, char *> or std::is_same_v<T, const char *>) {

      auto lt = array_interface::get_h5_lengths_type(g, name);

      H5_EXPECTS(lt.rank() == 1 + lt.has_complex_attribute);
      H5_EXPECTS(N == lt.lengths[0]);

      if constexpr (is_complex_v<T>) {
        // Allow reading compound hdf5 dataype into array<complex>
        if (hdf5_type_equal(lt.ty, hdf5_type<dcplx_t>())) {
          h5_read(g, name, reinterpret_cast<std::array<dcplx_t, N> &>(a));
          return;
        }

        // Allow to read non-complex data into array<complex>
        if (!lt.has_complex_attribute) {
          std::array<double, N> tmp;
          h5_read(g, name, tmp);
          std::copy(begin(tmp), end(tmp), begin(a));
          return;
        }
      }

      array_interface::h5_array_view v{hdf5_type<T>(), (void *)(a.data()), 1 /*rank*/, is_complex_v<T>};
      v.slab.count[0]  = N;
      v.slab.stride[0] = 1;
      v.L_tot[0]       = N;

      array_interface::read(g, name, v, lt);

    } else { // generic unknown type to hdf5
      auto g2 = g.open_group(name);

      // Check that shapes are compatible
      auto h5_shape = std::array<long, 1>{};
      h5_read(g2, "shape", h5_shape);
      H5_EXPECTS(N == h5_shape[0]);

      // Read using appropriate h5_read implementation
      for (int i = 0; i < N; ++i) h5_read(g2, std::to_string(i), a[i]);
    }
  }

} // namespace h5

#endif // LIBH5_STL_ARRAY_HPP
