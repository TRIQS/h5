// Copyright (c) 2019-2022 Simons Foundation
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

#ifndef LIBH5_GENERIC_HPP
#define LIBH5_GENERIC_HPP

#include <type_traits>

namespace h5 {

  /**
   * A generic read of a key in a group
   *
   * @tparam T The C++ type to be read
   * @param g The h5::group
   * @param key Name of the element
   * @return The value read from the file
   */
  template <typename T>
  T h5_read(group g, std::string const &key) {
    if constexpr (std::is_default_constructible_v<T>) {
      T x{};
      h5_read(g, key, x);
      return x;
    } else {
      return T::h5_read_construct(g, key);
    }
  }

  template <typename T>
  T read(group g, std::string const &key) {
    return h5_read<T>(g, key);
  }

  template <typename T>
  void read(group g, std::string const &key, T &x) {
    h5_read(g, key, x);
  }

  template <typename T>
  void write(group g, std::string const &key, T const &x) {
    h5_write(g, key, x);
  }

  /**
   * A generic attribute read from an object
   *
   * @tparam T The C++ type of the attribute
   * @param obj The object to read the attribute from
   * @param name Name of the attribute
   * @return The attribute object, and "" if the attribute does not exist.
   */
  template <typename T>
  T h5_read_attribute(object obj, std::string const &name) {
    T x;
    h5_read_attribute(obj, name, x);
    return x;
  }

  template <typename T>
  T read_attribute(group g, std::string const &key) {
    return h5_read_attribute<T>(g, key);
  }

  template <typename T>
  void read_attribute(group g, std::string const &key, T &x) {
    h5_read(g, key, x);
  }

  template <typename T>
  void write_attribute(group g, std::string const &key, T const &x) {
    h5_write(g, key, x);
  }

  /**
   * A generic attribute read from a key in an h5::group
   *
   * @tparam T The C++ type of the attribute
   * @param g The h5::group
   * @param key The key
   * @param name Name of the attribute
   * @return The attribute object, and "" if the attribute does not exist.
   */
  template <typename T>
  T h5_read_attribute_from_key(group g, std::string const &key, std::string const &name) {
    T x;
    h5_read_attribute_from_key(g, key, name, x);
    return x;
  }

  /**
   * Try a generic read of a key in a group.
   *
   * @tparam T The C++ type to be read
   * @param g  HDF5 group
   * @param key Name of the element
   * @param x Parameter to read.
   * @return true if the read succeeds, false if it fails
   */
  template <typename T>
  inline bool try_read(group g, std::string key, T &x) {
    if (g.has_key(key)) {
      h5_read(g, key, x);
      return true;
    }
    return false;
  }

} // namespace h5

#endif // LIBH5_GENERIC_HPP
