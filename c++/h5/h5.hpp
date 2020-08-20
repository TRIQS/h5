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

#ifndef LIBH5_H5_HPP
#define LIBH5_H5_HPP

#include "./std_addons/complex.hpp"

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

// Concept for cpp2py detection. Expected to be present in the lib in C++20 mode
#if (__cplusplus > 201703L)
namespace h5 {

  template <typename T>
  concept HasH5ReadWrite = requires(T const &x, T &y, h5::group g, std::string n) {
    {h5_write(g, n, x)};
    {h5_read(g, n, y)};
  };
} // namespace h5
#endif

#endif // LIBH5_H5_HPP
