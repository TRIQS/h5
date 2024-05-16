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
 * @brief Provides functions to read/write std::pair objects from/to HDF5.
 */

#ifndef LIBH5_STL_PAIR_HPP
#define LIBH5_STL_PAIR_HPP

#include "../format.hpp"
#include "../group.hpp"
#include "./string.hpp"

#include <string>
#include <utility>

namespace h5 {

  /**
   * @addtogroup rw_pair
   * @{
   */

  /// Specialization of h5::hdf5_format_impl for std::pair.
  template <typename T1, typename T2>
  struct hdf5_format_impl<std::pair<T1, T2>> {
    static std::string invoke() { return "PythonTupleWrap"; }
  };

  /**
   * @brief Write a std::pair to an HDF5 subgroup.
   *
   * @details Calls the specialized `h5_write` function for both values of the std::pair.
   *
   * @tparam T1 Value type #1.
   * @tparam T1 Value type #2.
   * @param g h5::group in which the subgroup is created.
   * @param name Name of the subgroup to which the std::pair is written.
   * @param p std::pair to be written.
   */
  template <typename T1, typename T2>
  void h5_write(group g, std::string const &name, std::pair<T1, T2> const &p) {
    auto gr = g.create_group(name);
    write_hdf5_format(gr, p);
    h5_write(gr, "0", p.first);
    h5_write(gr, "1", p.second);
  }

  /**
   * @brief Read a std::pair from an HDF5 subgroup.
   *
   * @details Calls the specialized `h5_read` function for both values of the std::pair.
   *
   * @tparam T1 Value type #1.
   * @tparam T1 Value type #2.
   * @param g h5::group containing the subgroup.
   * @param name Name of the subgroup from which the std::pair is read.
   * @param p std::pair to read into.
   */
  template <typename T1, typename T2>
  void h5_read(group g, std::string const &name, std::pair<T1, T2> &p) {
    auto gr = g.open_group(name);
    if (gr.get_all_subgroup_dataset_names().size() != 2)
      throw std::runtime_error("Error in h5::h5_read: Reading a std::pair from a group with more/less than 2 subgroups/datasets is not allowed");
    h5_read(gr, "0", p.first);
    h5_read(gr, "1", p.second);
  }

  /** @} */

} // namespace h5

#endif // LIBH5_STL_PAIR_HPP
