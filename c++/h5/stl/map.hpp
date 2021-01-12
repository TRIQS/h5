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

#pragma once
#include <map>
#include <type_traits>
#include "../group.hpp"
#include "./string.hpp"

namespace h5 {

  template <typename keyT, typename valueT>
  struct hdf5_format_impl<std::map<keyT, valueT>> {
    static std::string invoke() { return "Dict"; }
  };


  /**
   * Map of type keyT for the key and valueT for the value. keyT can be any 
   * class as long as it is writeable to h5 (an operator "<" is needed to 
   * be used in a map in the first place).
  */
  template <typename keyT, typename valueT>
  void h5_write(group f, std::string const &name, std::map<keyT, valueT> const &M) {
    auto gr = f.create_group(name);
    write_hdf5_format(gr, M);
    
    if constexpr (std::is_same_v<keyT, std::string>){
      for (auto &pvp : M) h5_write(gr, pvp.first, pvp.second);
    }
    else {
      int indx = 0;
      for (auto &pvp : M) {
        auto element_gr = gr.create_group( std::to_string(indx) );
        h5_write(element_gr, "key", pvp.first);
        h5_write(element_gr, "val", pvp.second);
        ++indx;
      }
    }
  }

  template <typename keyT, typename valueT>
  void h5_read(group f, std::string const &name, std::map<keyT, valueT> &M) {
    auto gr = f.open_group(name);
    M.clear();
    
    if constexpr (std::is_same_v<keyT, std::string>){
      for (auto const &x : gr.get_all_subgroup_dataset_names()) {
        valueT value;
        h5_read(gr, x, value);
        M.emplace(x, std::move(value));
      }
    }
    else {
      for (auto const &x : gr.get_all_subgroup_dataset_names()) {
        auto element_gr = gr.open_group(x);
        keyT key;
        valueT value;
        h5_read(element_gr, "key", key);
        h5_read(element_gr, "val", value);
        M.emplace(key, std::move(value));
      }
    }
  }

  /*
  template <typename T>
  void h5_write(group f, std::string const &name, std::map<std::string, T> const &M) {
    auto gr = f.create_group(name);
    write_hdf5_format(gr, M);
    
    for (auto &pvp : M) h5_write(gr, pvp.first, pvp.second);
  }

  template <typename T>
  void h5_read(group f, std::string const &name, std::map<std::string, T> &M) {
    auto gr = f.open_group(name);
    M.clear();
    
    for (auto const &x : gr.get_all_subgroup_dataset_names()) {
      T value;
      h5_read(gr, x, value);
      M.emplace(x, std::move(value));
    }

  }*/


} // namespace h5
