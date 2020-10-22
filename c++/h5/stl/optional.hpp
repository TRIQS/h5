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

#ifndef LIBH5_STL_OPTIONAL_HPP
#define LIBH5_STL_OPTIONAL_HPP

#include <optional>
#include "../group.hpp"
#include "./string.hpp"

namespace h5 {

  template <typename T>
  struct hdf5_format_impl<std::optional<T>> {
    static std::string invoke() { return hdf5_format_impl<T>::invoke(); }
  };

  /**
   * Optional : write if the value is set.
   */
  template <typename T>
  void h5_write(group gr, std::string const &name, std::optional<T> const &v) {
    if (bool(v)) h5_write(gr, name, *v);
  }

  /**
   * Read optional from the h5
   */
  template <typename T>
  void h5_read(group gr, std::string name, std::optional<T> &v) {
    v.reset();
    if (gr.has_key(name)) v.emplace(h5_read<T>(gr, name));
  }
} // namespace h5

#endif // LIBH5_STL_OPTIONAL_HPP
