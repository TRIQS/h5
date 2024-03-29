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

#ifndef LIBH5_FORMAT_HPP
#define LIBH5_FORMAT_HPP

#include "./macros.hpp"
#include "./group.hpp"
#include "./stl/string.hpp"
#include <complex>
#include <string>

namespace h5 {

  // a class T has either :
  //  1- a static member hdf5_format -> std::string (or a constexpr char * ?)
  //  2- specializes hdf5_format_impl
  // user function is get_hdf5_format <T>() in all cases.
  // A claass which is not default constructible :
  //  -- 1 : implement static T h5_read_construct(gr, name) : rebuilt  a new T
  //  -- 2 : NOT IMPLEMENTED : if we want to make it non intrusive,
  //  specialize with a struct similarly to hdf5_format_impl
  // to be implemented if needed.

  template <typename T>
  struct hdf5_format_impl {
    static std::string invoke() { return T::hdf5_format(); }
  };

#define H5_SPECIALIZE_FORMAT2(X, Y)                                                                                                                  \
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

  template <typename T>
  std::string get_hdf5_format() {
    return hdf5_format_impl<T>::invoke();
  }

  template <typename T>
  std::string get_hdf5_format(T const &) {
    return hdf5_format_impl<T>::invoke();
  }

  inline void write_hdf5_format_as_string(object obj, std::string const &s) { h5_write_attribute(obj, "Format", s); }

  // Write the h5 format tag to the object
  template <typename T>
  inline void write_hdf5_format(object obj, T const &) {
    h5_write_attribute(obj, "Format", get_hdf5_format<T>());
  }

  /// Read h5 format tag from the object
  void read_hdf5_format(object obj, std::string &s);
  std::string read_hdf5_format(group g);

  /// Add the h5 format tag to the key in the group
  void read_hdf5_format_from_key(group g, std::string const &key, std::string &s);

  /// Asserts that the tag of the group is the same as the given string. Throws std::runtime_error if incompatible
  void assert_hdf5_format_as_string(group g, const char *tag_expected, bool ignore_if_absent = false);

  /// Asserts that the tag of the group is the same as for T. Throws std::runtime_error if incompatible
  template <typename T>
  void assert_hdf5_format(group g, T const &, bool ignore_if_absent = false) {
    assert_hdf5_format_as_string(g, get_hdf5_format<T>().c_str(), ignore_if_absent);
  }

} // namespace h5

#endif // LIBH5_FORMAT_HPP
