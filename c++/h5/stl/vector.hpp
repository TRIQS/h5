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
 * @brief Provides functions to read/write std::vector objects from/to HDF5.
 */

#ifndef LIBH5_STL_VECTOR_HPP
#define LIBH5_STL_VECTOR_HPP

#include "./string.hpp"
#include "../complex.hpp"
#include "../format.hpp"
#include "../group.hpp"
#include "../scalar.hpp"
#include "../utils.hpp"

#include <string>
#include <vector>
#include <type_traits>

namespace h5 {

  namespace array_interface {

    /**
     * @ingroup rw_arrayinterface
     * @brief Create an h5::array_interface::h5_array_view for a std::vector.
     *
     * @tparam T Value type of std::vector.
     * @param v std::vector.
     * @return h5::array_interface::h5_array_view of rank 1.
     */
    template <typename T>
    h5_array_view h5_array_view_from_vector(std::vector<T> const &v) {
      h5_array_view res{hdf5_type<T>(), (void *)v.data(), 1, is_complex_v<std::decay_t<T>>};
      res.slab.count[0] = v.size();
      res.L_tot[0]      = v.size();
      return res;
    }

  } // namespace array_interface

  /**
   * @addtogroup rw_vector
   * @{
   */

  // Specialization of h5::hdf5_format_impl for std::vector<std::string>.
  H5_SPECIALIZE_FORMAT2(std::vector<std::string>, vector<string>);

  /// Specialization of h5::hdf5_format_impl for std::vector.
  template <typename T>
  struct hdf5_format_impl<std::vector<T>> {
    static std::string invoke() { return "List"; }
  };

  /**
   * @brief Create an h5::char_buf from a vector of strings.
   *
   * @param v Vector of strings.
   * @return h5::char_buf containing the strings.
   */
  char_buf to_char_buf(std::vector<std::string> const &v);

  /**
   * @brief Create an h5::char_buf from a vector of vectors of strings.
   *
   * @param v Vector of a vector of strings.
   * @return h5::char_buf containing the strings.
   */
  char_buf to_char_buf(std::vector<std::vector<std::string>> const &v);

  /**
   * @brief Create a vector of strings from an h5::char_buf.
   *
   * @param cb h5::char_buf.
   * @param v Vector of strings.
   */
  void from_char_buf(char_buf const &cb, std::vector<std::string> &v);

  /**
   * @brief Create a vector of vectors of strings from an h5::char_buf.
   *
   * @param cb h5::char_buf.
   * @param v Vector of vectors of strings.
   */
  void from_char_buf(char_buf const &cb, std::vector<std::vector<std::string>> &v);

  /**
   * @brief Write a std::vector to an HDF5 dataset/subgroup.
   *
   * @details Depending on the type of `T`, the following is written:
   * - If `T` is a simple type (arithmetic or complex), a 1d dataset is written.
   * - If `T` is `std::string`, an h5::char_buf is written, i.e. a 2d dataset of char with dimensions
   * (length of vector, max length of strings).
   * - Otherwise, it creates a subgroup and writes each element to the subgroup.
   *
   * @tparam T Value tupe of std::vector.
   * @param g h5::group in which the dataset/subgroup is created.
   * @param name Name of the dataset/subgroup to which the std::vector is written.
   * @param v std::vector to be written.
   */
  template <typename T>
  void h5_write(group g, std::string const &name, std::vector<T> const &v) {
    if constexpr (std::is_arithmetic_v<T> or is_complex_v<T>) {
      // vector of arithmetic/complex types
      array_interface::write(g, name, array_interface::h5_array_view_from_vector(v), true);
    } else if constexpr (std::is_same_v<T, std::string> or std::is_same_v<T, std::vector<std::string>>) {
      // vector (of vectors) of strings
      h5_write(g, name, to_char_buf(v));
    } else {
      // vector of generic types
      auto gr = g.create_group(name);
      write_hdf5_format(gr, v);
      for (int i = 0; i < v.size(); ++i) h5_write(gr, std::to_string(i), v[i]);
    }
  }

  /**
   * @brief Read a std::vector from an HDF5 dataset/subgroup.
   *
   * @details Depending on the type of `T`, the following is read:
   * - If `T` is a simple type (arithmetic or complex), a 1d dataset is read.
   * - If `T` is `std::string`, an h5::char_buf is read, i.e. a 2d dataset of char with dimensions
   * (length of vector, max length of strings).
   * - Otherwise, it opens a subgroup and reads each element from the subgroup.
   *
   * @tparam T Value tupe of std::vector.
   * @param g h5::group containing the dataset/subgroup.
   * @param name Name of the dataset/subgroup from which the std::vector is read.
   * @param v std::vector to read into.
   */
  template <typename T>
  void h5_read(group g, std::string name, std::vector<T> &v) {
    // throw exception if no link with the given name exists
    if (not g.has_key(name)) throw make_runtime_error("Error in h5_read: Dataset/Subgroup with name ", name, " does not exist");

    if (g.has_subgroup(name)) {
      // vector of generic type
      auto g2 = g.open_group(name);
      v.resize(g2.get_all_dataset_names().size() + g2.get_all_subgroup_names().size());
      for (int i = 0; i < v.size(); ++i) { h5_read(g2, std::to_string(i), v[i]); }
    } else {
      if constexpr (std::is_arithmetic_v<T> or is_complex_v<T>) {
        // vector of arithmetic/complex types
        auto lt = array_interface::get_h5_lengths_type(g, name);
        if (lt.rank() != 1 + is_complex_v<T>)
          throw make_runtime_error("Error in h5_read: Reading a vector from an array of rank ", lt.rank(), " is not allowed");
        v.resize(lt.lengths[0]);
        array_interface::read(g, name, array_interface::h5_array_view_from_vector(v), lt);
      } else if constexpr (std::is_same_v<T, std::string> or std::is_same_v<T, std::vector<std::string>>) {
        // vector of strings or vector of vector of strings
        char_buf cb;
        h5_read(g, name, cb);
        from_char_buf(cb, v);
      } else {
        // unsupported type
        throw make_runtime_error("Error in h5_read: HDF5 datatype not supported for reading into a std::vector");
      }
    }
  }

  /**
   * @brief Write a vector of vectors of strings to an HDF5 attribute.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param v Vector of vectors of strings to be written.
   */
  void h5_write_attribute(object obj, std::string const &name, std::vector<std::vector<std::string>> const &v);

  /**
   * @brief Read a vector of vectors of strings from an HDF5 attribute.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param v Vector of vectors of strings to read into.
   */
  void h5_read_attribute(object obj, std::string const &name, std::vector<std::vector<std::string>> &v);

  /**
   * @brief Write a vectors of strings to an HDF5 attribute.
   *
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param v Vector of strings to be written.
   */
  void h5_write_attribute(object obj, std::string const &name, std::vector<std::string> const &v);

  /**
   * @brief Read a vector of strings from an HDF5 attribute.
   *
   * @param obj Parent h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param v Vector of strings to read into.
   */
  void h5_read_attribute(object obj, std::string const &name, std::vector<std::string> &v);

  /** @} */

} // namespace h5

#endif // LIBH5_STL_VECTOR_HPP
