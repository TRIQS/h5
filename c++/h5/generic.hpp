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

/**
 * @file
 * @brief Provides a generic interface for reading/writing data from/to various HDF5 objects.
 * @details The generic functions (h5::read, h5::write, h5::read_attribute, ...) call their more specialized counterparts
 * (h5::h5_read, h5::h5_write, h5::h5_read_attribute, ...) which find the correct implementation using ADL.
 */

#ifndef LIBH5_GENERIC_HPP
#define LIBH5_GENERIC_HPP

#include "./group.hpp"

#include <string>
#include <type_traits>

namespace h5 {

  /**
   * @brief Generic implementation for reading from an HDF5 dataset/subgroup.
   *
   * @details It calls the static member function `T::h5_read_construct(group, T &)` in case `T` is not default constructible,
   * otherwise it calls the specialized `h5_read(group, std::string const &, T &)` with a default constructed `T` object.
   *
   * @tparam T C++ type to be read.
   * @param g h5::group containing the dataset/subgroup from which to read from.
   * @param key Name of the dataset/subgroup.
   * @return Data read from the dataset/subgroup.
   */
  template <typename T>
  T h5_read(group g, std::string const &key) {
    // default constructible
    if constexpr (std::is_default_constructible_v<T>) {
      T x{};
      h5_read(g, key, x);
      return x;
    } else { // not default constructible
      return T::h5_read_construct(g, key);
    }
  }

  /**
   * @brief Generic implementation for reading from an HDF5 dataset/subgroup.
   *
   * @details It simply calls h5::h5_read<T>(group, std::string const &).
   *
   * @tparam T C++ type to be read.
   * @param g h5::group containing the dataset/subgroup from which to read from.
   * @param key Name of the dataset/subgroup.
   * @return Data read from the dataset/subgroup.
   */
  template <typename T>
  T read(group g, std::string const &key) {
    return h5_read<T>(g, key);
  }

  /**
   * @brief Generic implementation for reading from an HDF5 dataset/subgroup into a given variable.
   *
   * @details It calls the specialized `h5_read(group, std::string const &, T &, auto const &...)` for the given `T`.
   *
   * @tparam T C++ type to be read.
   * @param g h5::group containing the dataset/subgroup from which to read from.
   * @param key Name of the dataset/subgroup.
   * @param x Variable to read into.
   * @param args Additional arguments to be passed to the specialized `h5_read(group, std::string const &, T &)` function.
   */
  template <typename T>
  void read(group g, std::string const &key, T &x, auto const &...args) {
    h5_read(g, key, x, args...);
  }

  /**
   * @brief Generic implementation for writing a variable to an HDF5 dataset/subgroup.
   *
   * @details It calls the specialized `h5_write(group, std::string const &, T const &)` for the given `T`.
   *
   * @tparam T C++ type to be written.
   * @param g h5::group in which the dataset/subgroup is created.
   * @param key Name of the dataset/subgroup to which the variable is written.
   * @param x Variable to be written.
   * @param args Additional arguments to be passed to the specialized `h5_write(group, std::string const &, T const &)` function.
   */
  template <typename T>
  void write(group g, std::string const &key, T const &x, auto const &...args) {
    h5_write(g, key, x, args...);
  }

  /**
   * @brief Generic implementation for reading an HDF5 attribute.
   *
   * @details `T` needs to be default constructible.
   *
   * @tparam T C++ type to be read.
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @return Data read from the attribute.
   */
  template <typename T>
  T h5_read_attribute(object obj, std::string const &name) {
    T x;
    h5_read_attribute(obj, name, x);
    return x;
  }

  /**
   * @brief Generic implementation for reading an HDF5 attribute.
   *
   * @details It simply calls h5::h5_read_attribute<T>(object, std::string const &).
   *
   * @tparam T C++ type to be read.
   * @param obj h5::object to which the attribute is attached.
   * @param key Name of the attribute.
   * @return Data read from the attribute.
   */
  template <typename T>
  T read_attribute(object obj, std::string const &key) {
    return h5_read_attribute<T>(obj, key);
  }

  /**
   * @brief Generic implementation for reading an HDF5 attribute into a given variable.
   *
   * @details It calls the specialized `h5_read_attribute(object, std::string const &, T &)` for the given `T`.
   *
   * @tparam T C++ type to be read.
   * @param obj h5::object to which the attribute is attached.
   * @param key Name of the attribute.
   * @param x Variable to read into.
   */
  template <typename T>
  void read_attribute(object obj, std::string const &key, T &x) {
    h5_read_attribute(obj, key, x);
  }

  /**
   * @brief Generic implementation for writing a variable to an HDF5 attribute.
   *
   * @details It calls the specialized `h5_write_attribute(object, std::string const &, T const &)` for the given `T`.
   *
   * @tparam T C++ type to be read.
   * @param obj h5::object to which the attribute is attached.
   * @param key Name of the attribute to be created.
   * @param x Variable to be written.
   */
  template <typename T>
  void write_attribute(object obj, std::string const &key, T const &x) {
    h5_write_attribute(obj, key, x);
  }

  /**
   * @brief Generic implementation for reading an HDF5 attribute.
   *
   * @details It calls the specialized `h5_read_attribute_from_key(object, std::string const &, std::string const &, T &)`
   * for the given `T`. `T` needs to be default constructible.
   *
   * @tparam T C++ type to be read.
   * @param g h5::group containing the HDF5 object to which the attribute is attached.
   * @param key Name of the object.
   * @param name Name of the attribute.
   * @return Data read from the attribute.
   */
  template <typename T>
  T h5_read_attribute_from_key(group g, std::string const &key, std::string const &name) {
    T x;
    h5_read_attribute_from_key(g, key, name, x);
    return x;
  }

  /**
   * @brief Check if an HDF5 dataset/subgroup with the given key exists in the given parent group before performing the read.
   *
   * @details If the key exists, it simply calls the specialized `h5_read(group, std::string const &, T &)` for the given `T`,
   * otherwise it does nothing.
   *
   * @tparam T C++ type to be read.
   * @param g h5::group containing the dataset/subgroup from which to read from.
   * @param key Name of the dataset/subgroup.
   * @param x Variable to read into.
   * @return True, if the dataset/subgroup exists and reading was successful, false otherwise.
   */
  template <typename T>
  inline bool try_read(group g, std::string const &key, T &x) {
    if (g.has_key(key)) {
      h5_read(g, key, x);
      return true;
    }
    return false;
  }

} // namespace h5

#endif // LIBH5_GENERIC_HPP
