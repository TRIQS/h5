// Copyright (c) 2019-2022 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell, chuffa

/**
 * @file
 * @brief Provides functions to read/write std::map objects from/to HDF5.
 */

#ifndef LIBH5_STL_MAP_HPP
#define LIBH5_STL_MAP_HPP

#include "../format.hpp"
#include "../group.hpp"
#include "./string.hpp"

#include <map>
#include <string>
#include <type_traits>
#include <utility>

namespace h5 {

  /**
   * @addtogroup rw_map
   * @{
   */

  /// Specialization of h5::hdf5_format_impl for std::map.
  template <typename keyT, typename valueT>
  struct hdf5_format_impl<std::map<keyT, valueT>> {
    static std::string invoke() { return "Dict"; }
  };

  /**
   * @brief Write a std::map to an HDF5 subgroup.
   *
   * @tparam keyT Key type of the std::map.
   * @tparam valueT Value type of the std::map.
   * @param g h5::group in which the subgroup is created.
   * @param name Name of the subgroup to which the std::map is written.
   * @param m std::map to be written.
   */
  template <typename keyT, typename valueT>
  void h5_write(group g, std::string const &name, std::map<keyT, valueT> const &m) {
    // create the subgroup and write the hdf5_format tag
    auto gr = g.create_group(name);
    write_hdf5_format(gr, m);

    // write element by element
    if constexpr (std::is_same_v<keyT, std::string>) {
      // if key is a string, use it for the dataset name
      for (auto const &[key, val] : m) h5_write(gr, key, val);
    } else {
      // otherwise, create a subgroup for each key-value pair
      int idx = 0;
      for (auto const &[key, val] : m) {
        auto element_gr = gr.create_group(std::to_string(idx));
        h5_write(element_gr, "key", key);
        h5_write(element_gr, "val", val);
        ++idx;
      }
    }
  }

  /**
   * @brief Read a std::map from an HDF5 subgroup.
   *
   * @tparam keyT Key type of the std::map.
   * @tparam valueT Value type of the std::map.
   * @param g h5::group containing the subgroup.
   * @param name Name of the subgroup from which the std::map is read.
   * @param m std::map to read into.
   */
  template <typename keyT, typename valueT>
  void h5_read(group g, std::string const &name, std::map<keyT, valueT> &m) {
    // open the subgroup and clear the map
    auto gr = g.open_group(name);
    m.clear();

    // loop over all subgroups and datasets in the current group
    for (auto const &x : gr.get_all_subgroup_dataset_names()) {
      valueT val;
      if constexpr (std::is_same_v<keyT, std::string>) {
        // if key is a string, read from the dataset with the same name
        h5_read(gr, x, val);
        m.emplace(x, std::move(val));
      } else {
        // otherwise, read from the subgroup
        auto element_gr = gr.open_group(x);
        keyT key;
        h5_read(element_gr, "key", key);
        h5_read(element_gr, "val", val);
        m.emplace(std::move(key), std::move(val));
      }
    }
  }

  /** @} */

} // namespace h5

#endif // LIBH5_STL_MAP_HPP
