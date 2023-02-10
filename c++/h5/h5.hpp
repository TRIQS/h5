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

#ifndef LIBH5_H5_HPP
#define LIBH5_H5_HPP

#include "./std_addons/complex.hpp"
#include <concepts>

#include "./file.hpp"
#include "./group.hpp"
#include "./format.hpp"
#include "./scalar.hpp"
#include "./stl/string.hpp"
#include "./stl/array.hpp"
#include "./stl/vector.hpp"
#include "./stl/map.hpp"
#include "./stl/pair.hpp"
#include "./stl/tuple.hpp"
#include "./stl/optional.hpp"
#include "./stl/variant.hpp"
#include "./generic.hpp"

// Define this so cpp2py modules know whether hdf5 was included
#define H5_INTERFACE_INCLUDED

// in some old version of hdf5 (Ubuntu 12.04 e.g.), the macro is not yet defined.
#ifndef H5_VERSION_GE

#define H5_VERSION_GE(Maj, Min, Rel)                                                                                                                 \
  (((H5_VERS_MAJOR == Maj) && (H5_VERS_MINOR == Min) && (H5_VERS_RELEASE >= Rel)) || ((H5_VERS_MAJOR == Maj) && (H5_VERS_MINOR > Min))               \
   || (H5_VERS_MAJOR > Maj))

#endif

namespace h5 {

  template <typename T>
  concept H5Storable = requires(T const &xc, T &x, h5::group g, std::string const &subgroup_name) {
                         { get_hdf5_format(xc) } -> std::convertible_to<std::string>;
                         { write(g, subgroup_name, xc) };
                         { read(g, subgroup_name, x) };
                       };

} // namespace h5

#endif // LIBH5_H5_HPP
