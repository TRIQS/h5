// Copyright (c) 2019-2024 Simons Foundation
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
// Authors: Thomas Hahn, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides functions to read/write std::optional objects from/to HDF5.
 */

#ifndef LIBH5_STL_OPTIONAL_HPP
#define LIBH5_STL_OPTIONAL_HPP

#include "../format.hpp"
#include "../group.hpp"
#include "./string.hpp"

#include <optional>
#include <string>

namespace h5 {

  /**
   * @addtogroup rw_optional
   * @{
   */

  /// Specialization of h5::hdf5_format_impl for std::optional.
  template <typename T>
  struct hdf5_format_impl<std::optional<T>> {
    static std::string invoke() { return hdf5_format_impl<T>::invoke(); }
  };

  /**
   * @brief Write a std::optional to an HDF5 dataset/subgroup (if it is set).
   *
   * @details Calls the specialized `h5_write` function for the value type of the std::optional.
   *
   * @tparam T Value type of std::optional.
   * @param g h5::group in which the dataset/subgroup is created.
   * @param name Name of the dataset/subgroup to which the std::optional value is written.
   * @param opt std::optional to be written.
   */
  template <typename T>
  void h5_write(group g, std::string const &name, std::optional<T> const &opt) {
    if (opt) h5_write(g, name, *opt);
  }

  /**
   * @brief Read a std::optional from an HDF5 dataset/subgroup.
   *
   * @details Calls the specialized `h5_read` function for the value type of the std::optional.
   *
   * @tparam T Value type of std::optional.
   * @param g h5::group containing the dataset/subgroup.
   * @param name Name of the dataset/subgroup from which the std::optional value is read.
   * @param opt std::optional to read into.
   */
  template <typename T>
  void h5_read(group g, std::string name, std::optional<T> &opt) {
    opt.reset();
    if (g.has_key(name)) opt.emplace(h5_read<T>(g, name));
  }

  /** @} */

} // namespace h5

#endif // LIBH5_STL_OPTIONAL_HPP
