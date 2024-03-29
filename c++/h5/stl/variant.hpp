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

#ifndef LIBH5_STL_VARIANT_HPP
#define LIBH5_STL_VARIANT_HPP

#include <variant>

#include "../group.hpp"
#include "./string.hpp"
#include "../generic.hpp"

namespace h5 {

  template <typename... T>
  struct hdf5_format_impl<std::variant<T...>> {
    static std::string invoke() = delete;
  };

  /**
   */
  template <typename... T>
  void h5_write(group gr, std::string const &name, std::variant<T...> const &v) {
    visit([&](auto const &x) { h5_write(gr, name, x); }, v);
  }

  template <typename VT, typename U, typename... T>
  void h5_read_variant_helper(VT &v, datatype dt, group gr, std::string const &name) {
    if (hdf5_type_equal(hdf5_type<U>(), dt)) {
      v = VT{h5_read<U>(gr, name)};
      return;
    }
    if constexpr (sizeof...(T) > 0)
      h5_read_variant_helper<VT, T...>(v, dt, gr, name);
    else
      throw std::runtime_error("Error in h5_read: std::variant<...> not compatible with Format \n");
  }

  /**
   * Read variant from the h5
   */
  template <typename... T>
  void h5_read(group gr, std::string const &name, std::variant<T...> &v) {
    // name is a group --> triqs object
    // assume for the moment, name is a dataset.
    dataset ds  = gr.open_dataset(name);
    datatype dt = get_hdf5_type(ds);
    h5_read_variant_helper<std::variant<T...>, T...>(v, dt, gr, name);
  }

} // namespace h5

#endif // LIBH5_STL_VARIANT_HPP
