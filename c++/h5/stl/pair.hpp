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

#ifndef LIBH5_STL_PAIR_HPP
#define LIBH5_STL_PAIR_HPP

#include <utility>
#include "../group.hpp"
#include "./string.hpp"

namespace h5 {

  template <typename T1, typename T2>
  struct hdf5_format_impl<std::pair<T1, T2>> {
    static std::string invoke() { return "PythonTupleWrap"; }
  };

  /**
   * Write Pair of T1 and T2 as a subgroup with numbers
   */
  template <typename T1, typename T2>
  void h5_write(group f, std::string const &name, std::pair<T1, T2> const &p) {
    auto gr = f.create_group(name);
    write_hdf5_format(gr, p);
    h5_write(gr, "0", p.first);
    h5_write(gr, "1", p.second);
  }

  /**
   * Read Pair of T1 and T2 from group
   */
  template <typename T1, typename T2>
  void h5_read(group f, std::string const &name, std::pair<T1, T2> &p) {
    auto gr = f.open_group(name);
    if (gr.get_all_subgroup_dataset_names().size() != 2)
      throw std::runtime_error("ERROR in std::pair h5_read: Incompatible number of group elements");
    h5_read(gr, "0", p.first);
    h5_read(gr, "1", p.second);
  }
} // namespace h5

#endif // LIBH5_STL_PAIR_HPP
