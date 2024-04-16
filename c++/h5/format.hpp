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
 * @brief Provides utilities for reading and writing `hdf5_format` tags.
 * @details An `hdf5_format` tag is a string that describes the type of object stored in an HDF5 file. A type `T` which is
 * HDF5 readable/writeable should do one of the following:
 * - implement a static member function `static hdf5_format() -> std::string` or
 * - specialize the `hdf5_format_impl` struct for `T` and provide an implementation for the member `static invoke() -> std::string`.
 */

#ifndef LIBH5_FORMAT_HPP
#define LIBH5_FORMAT_HPP

#include "./macros.hpp"
#include "./group.hpp"
#include "./stl/string.hpp"

#include <complex>
#include <string>

namespace h5 {

  /**
   * @brief Default type trait to get the `hdf5_format` tag of type `T` by calling its static member function
   * `T::hdf5_format()`.
   *
   * @tparam T Type for which the `hdf5_format` tag is to be retrieved.
   */
  template <typename T>
  struct hdf5_format_impl {
    static std::string invoke() { return T::hdf5_format(); }
  };

#define H5_SPECIALIZE_FORMAT2(X, Y)                                                                                                                  \
  /** @brief Specialization of h5::hdf5_format_impl for X. */                                                                                        \
  template <>                                                                                                                                        \
  struct hdf5_format_impl<X> {                                                                                                                       \
    static std::string invoke() { return H5_AS_STRING(Y); }                                                                                          \
  }

#define H5_SPECIALIZE_FORMAT(X) H5_SPECIALIZE_FORMAT2(X, X)

  H5_SPECIALIZE_FORMAT(bool);
  H5_SPECIALIZE_FORMAT(int);
  H5_SPECIALIZE_FORMAT(long);
  H5_SPECIALIZE_FORMAT(long long);
  H5_SPECIALIZE_FORMAT(unsigned int);
  H5_SPECIALIZE_FORMAT(unsigned long);
  H5_SPECIALIZE_FORMAT(unsigned long long);
  H5_SPECIALIZE_FORMAT(float);
  H5_SPECIALIZE_FORMAT(double);
  H5_SPECIALIZE_FORMAT(long double);
  H5_SPECIALIZE_FORMAT2(std::complex<double>, complex);

  /**
   * @brief Get the `hdf5_format` tag of type `T`.
   *
   * @tparam T Type for which the `hdf5_format` tag is to be retrieved..
   * @return std::string containing the `hdf5_format` tag.
   */
  template <typename T>
  std::string get_hdf5_format() {
    return hdf5_format_impl<T>::invoke();
  }

  /**
   * @brief Get the `hdf5_format` tag of type `T` using template argument deduction.
   *
   * @tparam T Type for which the `hdf5_format` tag is to be retrieved..
   * @return std::string containing the `hdf5_format` tag.
   */
  template <typename T>
  std::string get_hdf5_format(T const &) {
    return hdf5_format_impl<T>::invoke();
  }

  /**
   * @brief Write a std::string to an HDF5 attribute with the name 'Format'.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param s String to be written.
   */
  inline void write_hdf5_format_as_string(object obj, std::string const &s) { h5_write_attribute(obj, "Format", s); }

  /**
   * @brief Write an `hdf5_format` tag for type `T` to an HDF5 attribute with the name 'Format' using template
   * argument deduction.
   *
   * @tparam T Type for which the `hdf5_format` tag is to be written.
   * @param obj h5::object to which the attribute is attached.
   */
  template <typename T>
  inline void write_hdf5_format(object obj, T const &) {
    h5_write_attribute(obj, "Format", get_hdf5_format<T>());
  }

  /**
   * @brief Read an `hdf5_format` tag from an HDF5 attribute with the name 'Format'.
   *
   * @param obj h5::object from which the attribute is read.
   * @param s String to be read into.
   */
  void read_hdf5_format(object obj, std::string &s);

  /**
   * @brief Read an `hdf5_format` tag from an HDF5 attribute with the name 'Format' attached to a given h5::group.
   *
   * @param g h5::group from which the attribute is read.
   * @return String containing the `hdf5_format` tag.
   */
  std::string read_hdf5_format(group g);

  /**
   * @brief Read an `hdf5_format` tag from an HDF5 attribute with the name 'Format'.
   *
   * @param g h5::group containing the HDF5 object from which the attribute is read.
   * @param key Name of the object.
   * @param s String to be read into.
   */
  void read_hdf5_format_from_key(group g, std::string const &key, std::string &s);

  /**
   * @brief Assert that the `hdf5_format` tag attached to the given group is the same as the given tag.
   *
   * @details Throws a std::runtime_error if the tags don't match.
   *
   * @param g h5::group to be checked.
   * @param tag_expected Expected `hdf5_format` tag.
   * @param ignore_if_absent If true, the assertion is ignored if the group does not have a 'Format' attribute.
   */
  void assert_hdf5_format_as_string(group g, const char *tag_expected, bool ignore_if_absent = false);

  /**
   * @brief Assert that the `hdf5_format` tag attached to the given group is the same as the `hdf5_format` tag of the type `T`
   * using template argument deduction.
   *
   * @details Throws a std::runtime_error if the tags don't match.
   *
   * @tparam T Type for which the `hdf5_format` tag is to be checked.
   * @param g h5::group to be checked.
   * @param ignore_if_absent If true, the assertion is ignored if the group does not have a 'Format' attribute.
   */
  template <typename T>
  void assert_hdf5_format(group g, T const &, bool ignore_if_absent = false) {
    assert_hdf5_format_as_string(g, get_hdf5_format<T>().c_str(), ignore_if_absent);
  }

} // namespace h5

#endif // LIBH5_FORMAT_HPP
