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
 * @brief Provides a compound type and type traits for complex numbers.
 */

#ifndef LIBH5_COMPLEX_HPP
#define LIBH5_COMPLEX_HPP

#include <complex>
#include <type_traits>

namespace h5 {

  /**
   * @ingroup h5_types
   * @brief A complex compound type consisting of two doubles to represent a complex number.
   *
   * @details This type can be used to read/write complex numbers from/to HDF5 files.
   *
   * @note **h5** also supports the direct use of std::complex which is the recommended way to read/write complex data.
   */
  struct dcplx_t {
    /// Real part.
    double r;

    /// Imaginary part.
    double i;
  };

  // Type trait to check if a type is std::complex.
  template <typename T>
  struct _is_complex : std::false_type {};

  // Specialization of h5::_is_complex for std::complex.
  template <typename T>
  struct _is_complex<std::complex<T>> : std::true_type {};

  /**
   * @ingroup h5_types
   * @brief Boolean type trait set to true for std::complex types.
   * @tparam T Type to check.
   */
  template <typename T>
  constexpr bool is_complex_v = _is_complex<T>::value;

} // namespace h5

#endif // LIBH5_COMPLEX_HPP
