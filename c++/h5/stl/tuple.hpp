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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides functions to read/write std::tuple object from/to HDF5.
 */

#ifndef LIBH5_STL_TUPLE_HPP
#define LIBH5_STL_TUPLE_HPP

#include "../format.hpp"
#include "../group.hpp"
#include "./string.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

namespace h5 {

  /// Specialization of h5::hdf5_format_impl for std::tuple.
  template <typename... T>
  struct hdf5_format_impl<std::tuple<T...>> {
    static std::string invoke() { return "PythonTupleWrap"; }
  };

  namespace detail {

    // Helper function to write a tuple to HDF5.
    template <typename... Ts, std::size_t... Is>
    void h5_write_tuple_impl(group g, std::string const &, std::tuple<Ts...> const &tup, std::index_sequence<Is...>) {
      (h5_write(g, std::to_string(Is), std::get<Is>(tup)), ...);
    }

    // Helper function to read a tuple from HDF5.
    template <typename... Ts, std::size_t... Is>
    void h5_read_tuple_impl(group g, std::string const &, std::tuple<Ts...> &tup, std::index_sequence<Is...>) {
      if (g.get_all_subgroup_dataset_names().size() != sizeof...(Is))
        throw std::runtime_error(
           "Error in h5_read_tuple_impl: Reading a std::tuple<Ts...> from a group with more/less than sizeof...(Ts) subgroups/datasets is not allowed");
      (h5_read(g, std::to_string(Is), std::get<Is>(tup)), ...);
    }

  } // namespace detail

  /**
   * @brief Write a std::tuple to an HDF5 subgroup.
   *
   * @details Calls the specialized `h5_write` function for every element of the std::tuple.
   *
   * @tparam Ts Tuple types.
   * @param g h5::group in which the subgroup is created.
   * @param name Name of the subgroup to which the std::tuple is written.
   * @param tup std::tuple to be written.
   */
  template <typename... Ts>
  void h5_write(group g, std::string const &name, std::tuple<Ts...> const &tup) {
    auto gr = g.create_group(name);
    write_hdf5_format(gr, tup);
    detail::h5_write_tuple_impl(gr, name, tup, std::index_sequence_for<Ts...>{});
  }

  /**
   * @brief Read a std::tuple from an HDF5 subgroup.
   *
   * @details Calls the specialized `h5_read` function for every value of the std::tuple.
   *
   * @tparam Ts Tuple types.
   * @param g h5::group containing the subgroup.
   * @param name Name of the subgroup from which the std::tuple is read.
   * @param tup std::tuple to read into.
   */
  template <typename... Ts>
  void h5_read(group g, std::string const &name, std::tuple<Ts...> &tup) {
    auto gr = g.open_group(name);
    detail::h5_read_tuple_impl(gr, name, tup, std::index_sequence_for<Ts...>{});
  }

} // namespace h5

#endif // LIBH5_STL_TUPLE_HPP
