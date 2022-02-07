// Copyright (c) 2020 Simons Foundation
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

#include <h5/h5.hpp>
#include <string>
#include <vector>

class storable {
  public:
  // Some Data members
  std::vector<int> vec = {4, 2};
  std::string s        = "😎";

  // Default constructor has to be specified for cpp2py to pick it up
  storable() = default;

  // Our format string
  static std::string hdf5_format() { return "Storable"; }

  friend void h5_write(h5::group g, std::string const &name, storable const &obj) {
    auto gr = g.create_group(name);
    write_hdf5_format(gr, obj);
    h5_write(gr, "vec", obj.vec);
    h5_write(gr, "s", obj.s);
  }

  friend void h5_read(h5::group g, std::string const &name, storable &obj) {
    auto gr = g.open_group(name);
    h5_read(gr, "vec", obj.vec);
    h5_read(gr, "s", obj.s);
  }
};
