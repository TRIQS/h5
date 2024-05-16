// Copyright (c) 2024 Simons Foundation
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
// Authors: Thomas Hahn, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides some utility functions for h5.
 */

#ifndef LIBH5_UTILS_HPP
#define LIBH5_UTILS_HPP

#include <cstdint>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace h5 {

  /**
   * @addtogroup utilities
   * @{
   */

  /**
   * @brief ID type used in HDF5.
   *
   * @details This is just a copy from the HDF5 library (see the official
   * <a href="https://docs.hdfgroup.org/hdf5/develop/_h5_ipublic_8h.html#a0045db7ff9c22ad35db6ae91662e1943">documentation</a>).
   * It is used to completely isolate our header from the HDF5 headers. In the object.cpp file a `static_assert` is used to verify
   * its validity.
   */
  using hid_t = int64_t;

  /**
   * @brief Size type used in HDF5.
   * @details This is just a copy from the HDF5 library. It is used to completely isolate our header from the HDF5 headers.
   * In the object.cpp file a `static_assert` is used to verify its validity.
   */
#ifdef H5_VER_GE_113
  using hsize_t = uint64_t;
#else
  using hsize_t = unsigned long long;
#endif

  /// Vector of h5::hsize_t used throughout the h5 library.
  using v_t = std::vector<hsize_t>;

  /**
   * @brief Create a std::runtime_error with an error message constructed from the given arguments.
   *
   * @tparam Ts Types of the arguments.
   * @param ts Arguments streamed into the error message string.
   * @return std::runtime_error.
   */
  template <typename... Ts>
  [[nodiscard]] std::runtime_error make_runtime_error(Ts const &...ts) {
    std::stringstream ss;
    (ss << ... << ts);
    return std::runtime_error{ss.str()};
  }

  /** @} */

} // namespace h5

#endif // LIBH5_UTILS_HPP
