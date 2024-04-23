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
 * @brief Provides a generic interface to read/write scalars from/to HDF5.
 */

#ifndef LIBH5_SCALAR_HPP
#define LIBH5_SCALAR_HPP

#include "./array_interface.hpp"
#include "./complex.hpp"
#include "./group.hpp"
#include "./macros.hpp"
#include "./object.hpp"

#include <cmath>
#include <string>
#include <type_traits>

namespace h5 {

  namespace array_interface {

    /**
     * @ingroup rw_arrayinterface
     * @brief Create an array view for a scalar.
     *
     * @tparam T Scalar type.
     * @param x Scalar value.
     * @return h5::array_interface::array_view of rank 0.
     */
    template <typename T>
    array_view array_view_from_scalar(T &x) {
      return {hdf5_type<std::decay_t<T>>(), (void *)(&x), 0, is_complex_v<std::decay_t<T>>};
    }

  } // namespace array_interface

  /**
   * @addtogroup rw_scalar
   * @{
   */

  /**
   * @brief Write a scalar to an HDF5 dataset.
   *
   * @details The scalar type needs to be either arithmetic, complex or of type h5::dcplx_t.
   *
   * @tparam T Scalar type.
   * @param g h5::group in which the dataset is created.
   * @param name Name of the dataset.
   * @param x Scalar value to be written.
   */
  template <typename T>
  void h5_write(group g, std::string const &name, T const &x) H5_REQUIRES(std::is_arithmetic_v<T> or is_complex_v<T> or std::is_same_v<T, dcplx_t>) {
    array_interface::write(g, name, array_interface::array_view_from_scalar(x), false);
  }

  /**
   * @brief Read a scalar from an HDF5 dataset.
   *
   * @details The scalar type needs to be either arithmetic, complex or of type h5::dcplx_t.
   *
   * @tparam T Scalar type.
   * @param g h5::group containing the dataset.
   * @param name Name of the dataset.
   * @param x Scalar variable to be read into.
   */
  template <typename T>
  void h5_read(group g, std::string const &name, T &x) H5_REQUIRES(std::is_arithmetic_v<T> or is_complex_v<T> or std::is_same_v<T, dcplx_t>) {
    // backward compatibility to read complex values stored the old way (in a subgroup)
    if constexpr (is_complex_v<T>) {
      if (g.has_subgroup(name)) {
        group gr = g.open_group(name);
        H5_ASSERT(gr.has_key("r") and gr.has_key("i"));
        double r = NAN, i = NAN;
        h5_read(gr, "r", r);
        h5_read(gr, "i", i);
        x = std::complex<double>{r, i};
        return;
      }
    }

    // get dataset_info
    auto ds_info = array_interface::get_dataset_info(g, name);

    // read complex values stored as a compound HDF5 datatype
    if constexpr (is_complex_v<T>) {
      if (hdf5_type_equal(ds_info.ty, hdf5_type<dcplx_t>())) {
        h5_read(g, name, reinterpret_cast<dcplx_t &>(x)); // NOLINT (reinterpret_cast is safe here)
        return;
      }
    }

    // read scalar value
    array_interface::read(g, name, array_interface::array_view_from_scalar(x));
  }

  /**
   * @brief Write a scalar to an HDF5 attribute.
   *
   * @details The scalar type needs to be either arithmetic or std::complex.
   *
   * @tparam T Scalar type.
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param x Scalar value to be written.
   */
  template <typename T>
  void h5_write_attribute(object obj, std::string const &name, T const &x) H5_REQUIRES(std::is_arithmetic_v<T> or is_complex_v<T>) {
    array_interface::write_attribute(obj, name, array_interface::array_view_from_scalar(x));
  }

  /**
   * @brief Read a scalar from an HDF5 attribute.
   *
   * @details The scalar type needs to be either arithmetic or std::complex.
   *
   * @tparam T Scalar type.
   * @param obj h5::object to which the attribute is attached.
   * @param name Name of the attribute.
   * @param x Scalar variable to be read into.
   */
  template <typename T>
  void h5_read_attribute(object obj, std::string const &name, T &x) H5_REQUIRES(std::is_arithmetic_v<T> or is_complex_v<T>) {
    array_interface::read_attribute(obj, name, array_interface::array_view_from_scalar(x));
  }

  /** @} */

} // namespace h5

#endif // LIBH5_SCALAR_HPP
