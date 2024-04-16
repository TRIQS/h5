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
 * @brief Provides some utility functions for h5.
 */

#ifndef LIBH5_UTILS_HPP
#define LIBH5_UTILS_HPP

#include <stdexcept>
#include <sstream>

namespace h5 {

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

} // namespace h5

#endif // LIBH5_UTILS_HPP
