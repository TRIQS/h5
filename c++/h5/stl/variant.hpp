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
 * @brief Provides functions to read/write std::variant object from/to HDF5.
 */

#ifndef LIBH5_STL_VARIANT_HPP
#define LIBH5_STL_VARIANT_HPP

#include "../format.hpp"
#include "../generic.hpp"
#include "../group.hpp"
#include "./string.hpp"

#include <stdexcept>
#include <string>
#include <variant>

namespace h5 {

  /**
   * @addtogroup rw_variant
   * @{
   */

  /// Specialization of h5::hdf5_format_impl for std::variant.
  template <typename... T>
  struct hdf5_format_impl<std::variant<T...>> {
    static std::string invoke() = delete;
  };

  /**
   * @brief Write a std::variant to an HDF5 dataset/subgroup.
   *
   * @details Calls the specialized `h5_write` for the type currently stored in the std::variant.
   *
   * @tparam Ts Variant types.
   * @param g h5::group in which the dataset/subgroup is created.
   * @param name Name of the dataset/subgroup to which the std::variant is written.
   * @param v std::variant to be written.
   */
  template <typename... Ts>
  void h5_write(group g, std::string const &name, std::variant<Ts...> const &v) {
    std::visit([&](auto const &x) { h5_write(g, name, x); }, v);
  }

  // Helper function to read a std::variant from HDF5.
  template <typename VT, typename U, typename... Ts>
  void h5_read_variant_helper(VT &v, datatype dt, group g, std::string const &name) {
    // finds the correct h5_read recursively
    if (hdf5_type_equal(hdf5_type<U>(), dt)) {
      v = VT{h5_read<U>(g, name)};
      return;
    }
    if constexpr (sizeof...(Ts) > 0)
      h5_read_variant_helper<VT, Ts...>(v, dt, g, name);
    else
      throw std::runtime_error("Error in h5_read_variant_helper: Type stored in the variant has no corresponding HDF5 datatype");
  }

  /**
   * @brief Read a std::variant from an HDF5 dataset.
   *
   * @warning This function only works, if name points to a dataset and not a group. Depending on the HDF5 datatype
   * of the dataset, it calls the specialized `h5_read`.
   *
   * @tparam Ts Variant types.
   * @param g h5::group containing the dataset.
   * @param name Name of the dataset from which the std::variant is read.
   * @param v std::variant to read into.
   */
  template <typename... Ts>
  void h5_read(group g, std::string const &name, std::variant<Ts...> &v) {
    // name is a group --> triqs object
    // assume for the moment, name is a dataset.
    dataset ds  = g.open_dataset(name);
    datatype dt = get_hdf5_type(ds);
    h5_read_variant_helper<std::variant<Ts...>, Ts...>(v, dt, g, name);
  }

  /** @} */

} // namespace h5

#endif // LIBH5_STL_VARIANT_HPP
