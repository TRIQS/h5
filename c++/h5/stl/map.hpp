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

#ifndef LIBH5_STL_MAP_HPP
#define LIBH5_STL_MAP_HPP

#include <map>
#include "../group.hpp"
#include "./string.hpp"

namespace h5 {

  template <typename T>
  struct hdf5_format_impl<std::map<std::string, T>> {
    static std::string invoke() { return "Dict"; }
  };

  /**
   * Map of string and T as a subgroup with key_names
   */
  template <typename T>
  void h5_write(group f, std::string const &name, std::map<std::string, T> const &M) {
    auto gr = f.create_group(name);
    write_hdf5_format(gr, M);
    for (auto &pvp : M) h5_write(gr, pvp.first, pvp.second);
  }

  /**
   * Map of string and T
   */
  template <typename T>
  void h5_read(group f, std::string const &name, std::map<std::string, T> &M) {
    auto gr = f.open_group(name);
    M.clear();
    for (auto const &x : gr.get_all_subgroup_dataset_names()) {
      T value;
      h5_read(gr, x, value);
      M.emplace(x, std::move(value));
    }
  }

} // namespace h5

#endif // LIBH5_STL_MAP_HPP
