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

#ifndef LIBH5_STL_TUPLE_HPP
#define LIBH5_STL_TUPLE_HPP

#include <tuple>
#include "../group.hpp"
#include "./string.hpp"

namespace h5 {

  template <typename... T>
  struct hdf5_format_impl<std::tuple<T...>> {
    static std::string invoke() { return "PythonTupleWrap"; }
  };

  namespace detail {
    template <typename... T, std::size_t... Is>
    void h5_write_tuple_impl(group gr, std::string const &, std::tuple<T...> const &tpl, std::index_sequence<Is...>) {
      (h5_write(gr, std::to_string(Is), std::get<Is>(tpl)), ...);
    }

    template <typename... T, std::size_t... Is>
    void h5_read_tuple_impl(group gr, std::string const &, std::tuple<T...> &tpl, std::index_sequence<Is...>) {
      if (gr.get_all_subgroup_dataset_names().size() != sizeof...(Is))
        throw std::runtime_error("ERROR in std::tuple h5_read: Tuple size incompatible to number of group elements");
      (h5_read(gr, std::to_string(Is), std::get<Is>(tpl)), ...);
    }

  } // namespace detail

  /**
   * Tuple of T... as a subgroup with numbers
   */
  template <typename... T>
  void h5_write(group f, std::string const &name, std::tuple<T...> const &tpl) {
    auto gr = f.create_group(name);
    write_hdf5_format(gr, tpl);
    detail::h5_write_tuple_impl(gr, name, tpl, std::index_sequence_for<T...>{});
  }

  /**
   * Tuple of T...
   */
  template <typename... T>
  void h5_read(group f, std::string const &name, std::tuple<T...> &tpl) {
    auto gr = f.open_group(name);
    detail::h5_read_tuple_impl(gr, name, tpl, std::index_sequence_for<T...>{});
  }
} // namespace h5

#endif // LIBH5_STL_TUPLE_HPP
