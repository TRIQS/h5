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

#include "./format.hpp"
#include "./stl/string.hpp"
#include <string>

namespace h5 {

  void read_hdf5_format(object obj, std::string &s) {
    h5_read_attribute(obj, "Format", s);
    if (s == "") { // Backward compatibility
      h5_read_attribute(obj, "TRIQS_HDF5_data_scheme", s);
    }
  }

  std::string read_hdf5_format(group g) {
    std::string s;
    read_hdf5_format(g, s);
    return s;
  }

  void read_hdf5_format_from_key(group g, std::string const &key, std::string &s) {
    h5_read_attribute_from_key(g, key, "Format", s);
    if (s == "") { // Backward compatibility
      h5_read_attribute_from_key(g, key, "TRIQS_HDF5_data_scheme", s);
    }
  }

  void assert_hdf5_format_as_string(group g, const char *tag_expected, bool ignore_if_absent) {
    auto tag_file = read_hdf5_format(g);
    if (ignore_if_absent and tag_file.empty()) return;
    if (tag_file != tag_expected)
      throw std::runtime_error("h5_read : mismatch of the Format tag in the h5 group : found " + tag_file + " while I expected " + tag_expected);
  }

} // namespace h5
