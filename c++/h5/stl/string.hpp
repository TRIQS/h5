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
#include "../group.hpp"
#include <string>

namespace h5 {

  template <typename T>
  struct hdf5_format_impl;

  template <>
  struct hdf5_format_impl<std::string> {
    static std::string invoke() { return "string"; }
  };

  /**
   * Write a string into an h5::group
   *
   * Format : Fixed size string
   *
   * @param g The h5::group
   * @param name The name of the dataset
   * @param s String to be saved.
   */
  void h5_write(group g, std::string const &name, std::string const &s);

  /// Write a const char array as a string
  inline void h5_write(group g, std::string const &name, const char *s) { h5_write(g, name, std::string{s}); }

  /**
   * Read a string from an h5::group
   *
   * @param g The h5::group
   * @param name The name of the dataset
   * @param s The string to read into
   */
  void h5_read(group g, std::string const &name, std::string &s);

  // Explicitly forbidden.
  inline void h5_read(group g, std::string const &name, char *s) = delete;

  /**
   * Write a string attribute to an object
   *
   * @param obj The object to write the attribute to
   * @param name The name of the attribute
   * @param s The string attribute
  */
  void h5_write_attribute(object obj, std::string const &name, std::string const &s);

  /// Write a const char array as a string attribute of an h5::object
  inline void h5_write_attribute(object obj, std::string const &name, const char *s) { h5_write_attribute(obj, name, std::string{s}); }

  /**
   * Read a string attribute from an object
   *
   * @param obj The object to read the attribute from
   * @param name The name of the attribute
   * @param value The string to read into
   */
  void h5_read_attribute(object obj, std::string const &name, std::string &s);

  // Explicitly forbidden
  inline void h5_read_attribute(object obj, std::string const &name, char *s) = delete;

  /**
   * Write a string attribute to a key in an h5::group
   *
   * @param g The h5::group
   * @param key The key
   * @param name The name of the attribute
   * @param s The string attribute
  */
  void h5_write_attribute_to_key(group g, std::string const &key, std::string const &name, std::string const &s);

  /// Write a const char array as a string attribute of a particular key
  inline void h5_write_attribute_to_key(group g, std::string const &key, std::string const &name, const char *s) {
    h5_write_attribute_to_key(g, key, name, std::string{s});
  }

  /**
   * Read a string attribute from a key in an h5::group
   *
   * @param g The h5::group
   * @param key The key
   * @param name The name of the attribute
   * @param s The string to read into
  */
  void h5_read_attribute_from_key(group g, std::string const &key, std::string const &name, std::string &s);

  // ---------------------   char_buf -----------------------

  // char_buf contains an n dimensional array of strings as fixed size strings, flatten in a 1d array of char.
  // the last dimension is the max length of the strings + 1, because of the ending 0 in C !
  struct char_buf {
    std::vector<char> buffer;
    v_t lengths;

    // the string datatype
    [[nodiscard]] datatype dtype() const;

    // the dataspace (without last dim, which is the string).
    [[nodiscard]] dataspace dspace() const;
  };

  // read/write for char_buf
  void h5_write(group g, std::string const &name, char_buf const &cb);
  void h5_write_attribute(object obj, std::string const &name, char_buf const &cb);
  void h5_read(group g, std::string const &name, char_buf &_cb);
  void h5_read_attribute(object obj, std::string const &name, char_buf &_cb);

} // namespace h5
