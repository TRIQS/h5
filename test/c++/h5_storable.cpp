// Copyright (c) 2024 Simons Foundation
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
// Authors: Thomas Hahn

#include <gtest/gtest.h>
#include <h5/h5.hpp>

#include <string>

// Non-storable type.
struct non_storable {};

// Non-storable type with h5_write.
struct non_storable_with_h5_write {
  friend void h5_write(h5::group, std::string const &, non_storable_with_h5_write const &) {}
};

// Non-storable type with h5_read.
struct non_storable_with_h5_read {
  friend void h5_read(h5::group, std::string const &, non_storable_with_h5_read &) {}
};

// Non-storable type with both h5_write and h5_read.
struct non_storable_with_both {
  friend void h5_write(h5::group, std::string const &, non_storable_with_both const &) {}
  friend void h5_read(h5::group, std::string const &, non_storable_with_both &) {}
};

// Storable type defining a static hdf5_format member function.
struct storable_with_hdf5_format {
  static std::string hdf5_format() { return "storable_with_hdf5_format"; }
  friend void h5_write(h5::group, std::string const &, storable_with_hdf5_format const &) {}
  friend void h5_read(h5::group, std::string const &, storable_with_hdf5_format &) {}
};

// Storable type defining a static h5_read_construct static member function.
struct storable_with_h5_read_construct {
  static std::string hdf5_format() { return "storable_with_hdf5_format"; }
  friend void h5_write(h5::group, std::string const &, storable_with_h5_read_construct const &) {}
  static auto h5_read_construct(h5::group, std::string const &) { return storable_with_h5_read_construct{}; }
};

TEST(H5, Storable) {
  static_assert(!h5::Storable<non_storable>);
  static_assert(!h5::Storable<non_storable_with_h5_write>);
  static_assert(!h5::Storable<non_storable_with_h5_read>);
  static_assert(!h5::Storable<non_storable_with_both>);
  static_assert(h5::Storable<storable_with_hdf5_format>);
  static_assert(h5::Storable<storable_with_h5_read_construct>);
};
