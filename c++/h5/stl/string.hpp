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

/**
 * @file
 * @brief Provides functions to read/write std::string, char* and h5::char_buf objects from/to HDF5.
 */

#ifndef LIBH5_STL_STRING_HPP
#define LIBH5_STL_STRING_HPP

#include "../group.hpp"

#include <string>
#include <vector>

namespace h5 {

  // Forward declaration.
  template <typename T>
  struct hdf5_format_impl;

  /// Specialization of h5::hdf5_format_impl for std::string.
  template <>
  struct hdf5_format_impl<std::string> {
    static std::string invoke() { return "string"; }
  };

  /**
   * @brief Write a std::string to an HDF5 dataset.
   *
   * @param g h5::group in which the dataset is created.
   * @param name Name of the dataset.
   * @param s std::string to be written.
   */
  void h5_write(group g, std::string const &name, std::string const &s);

  /**
   * @brief Write a `const char*` to an HDF5 dataset.
   *
   * @param g h5::group in which the dataset is created.
   * @param name Name of the dataset.
   * @param s `const char*` to be written.
   */
  inline void h5_write(group g, std::string const &name, const char *s) { h5_write(g, name, std::string{s}); }

  /**
   * @brief Read a string from an HDF5 dataset into a std::string.
   *
   * @param g h5::group containing the dataset.
   * @param name Name of the dataset.
   * @param s std::string to read into.
   */
  void h5_read(group g, std::string const &name, std::string &s);

  /**
   * @brief Read a string from an HDF5 dataset into a `char*`.
   * @warning Reading into a `char*` is not allowed. Use h5::h5_read(group, std::string const &, std::string &) instead.
   */
  inline void h5_read(group g, std::string const &name, char *s) = delete;

  /**
   * @brief Write a std::string to an HDF5 attribute.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param s std::string to be written.
   */
  void h5_write_attribute(object obj, std::string const &name, std::string const &s);

  /**
   * @brief Write a `const char*` to an HDF5 attribute.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param s 'const char*' to be written.
   */
  inline void h5_write_attribute(object obj, std::string const &name, const char *s) { h5_write_attribute(obj, name, std::string{s}); }

  /**
   * @brief Read a string from an HDF5 attribute into a std::string.
   *
   * @details If the attribute does not exist, an empty string is returned.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param s std::string to read into.
   */
  void h5_read_attribute(object obj, std::string const &name, std::string &s);

  /**
   * @brief Read a string from an HDF5 attribute into a `char*`.
   * @warning Reading into a `char*` is not allowed. Use h5::h5_read_attribute(object, std::string const &, std::string &) instead.
   */
  inline void h5_read_attribute(object obj, std::string const &name, char *s) = delete;

  /**
   * @brief Write a std::string to an HDF5 attribute.
   *
   * @param g h5::group containing the HDF5 object to which the attribute is attached.
   * @param key Name of the object.
   * @param name Name of the attribute.
   * @param s std::string to be written.
   */
  void h5_write_attribute_to_key(group g, std::string const &key, std::string const &name, std::string const &s);

  /**
   * @brief Write a `const char*` to an HDF5 attribute.
   *
   * @param g h5::group containing the HDF5 object to which the attribute is attached.
   * @param key Name of the object.
   * @param name Name of the attribute.
   * @param s `const char*` to be written.
   */
  inline void h5_write_attribute_to_key(group g, std::string const &key, std::string const &name, const char *s) {
    h5_write_attribute_to_key(g, key, name, std::string{s});
  }

  /**
   * @brief Read a string from an HDF5 attribute into a std::string.
   *
   * @details If the attribute does not exist, an empty string is returned.
   *
   * @param g h5::group containing the HDF5 object to which the attribute is attached.
   * @param key Name of the object.
   * @param name Name of the attribute.
   * @param s std::string to read into.
   */
  void h5_read_attribute_from_key(group g, std::string const &key, std::string const &name, std::string &s);

  /**
   * @brief Stores an arbitrary number of strings in a 1-dimensional std::vector<char>.
   *
   * @details Each string is assumed to have the same length. If a string is shorter than this length, it is padded
   * with zeros. The `lengths` member should have the following entries: the number of strings in each dimension and
   * the max. allowed length of the strings + 1. For example, if the original strings are stored in a 2-dimensional
   * array of size `MxN` and the longest string is of length `L`, then `lengths = {M, N, L+1}`.
   *
   * The HDF5 datatype is a fixed-length string of size `lengths.back()` and the HDF5 dataspace is an n-dimensional
   * array of fixed-sized strings.
   */
  struct char_buf {
    /// Stores strings in a 1-dimensional vector.
    std::vector<char> buffer;

    /// Stores the number of strings in each dimension and the max. allowed length of the strings + 1.
    v_t lengths;

    /// Get the HDF5 datatype.
    [[nodiscard]] datatype dtype() const;

    /// Get the HDF5 dataspace.
    [[nodiscard]] dataspace dspace() const;
  };

  /**
   * @brief Write an h5::char_buf to an HDF5 dataset.
   *
   * @param g h5::group in which the dataset is created.
   * @param name Name of the dataset.
   * @param cb h5::char_buf to be written.
   */
  void h5_write(group g, std::string const &name, char_buf const &cb);

  /**
   * @brief Read an h5::char_buf from an HDF5 dataset.
   *
   * @param g h5::group containing the dataset.
   * @param name Name of the dataset.
   * @param cb h5::char_buf to read into.
   */
  void h5_read(group g, std::string const &name, char_buf &cb);

  /**
   * @brief Write an h5::char_buf to an HDF5 attribute.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param cb h5::char_buf to be written.
   */
  void h5_write_attribute(object obj, std::string const &name, char_buf const &cb);

  /**
   * @brief Read an h5::char_buf from an HDF5 attribute.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param cb h5::char_buf to read into.
   */
  void h5_read_attribute(object obj, std::string const &name, char_buf &cb);

} // namespace h5

#endif // LIBH5_STL_STRING_HPP
