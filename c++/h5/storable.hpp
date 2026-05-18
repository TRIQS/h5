// Copyright (c) 2019-2024 Simons Foundation
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
 * @brief Provides the Storable concept to check if a type can be read/written from/to HDF5.
 */

#ifndef LIBH5_STORABLE_HPP
#define LIBH5_STORABLE_HPP

#include "./format.hpp"
#include "./group.hpp"

#include <concepts>
#include <string>

namespace h5 {

  /**
   * @ingroup utilities
   * @brief Concept to check if a type can be read/written from/to HDF5.
   * @tparam T Type to check.
   */
  template <typename T>
  concept Storable = requires(T const &xc, h5::group g, std::string const &name) {
    { T::hdf5_format() } -> std::convertible_to<std::string>;
    { h5_write(g, name, xc) };
  } && (requires(T &x, h5::group g, std::string const &name) {
    { h5_read(g, name, x) };
  } || requires(h5::group g, std::string const &name) {
    { T::h5_read_construct(g, name) };
  });

} // namespace h5

#endif // LIBH5_STORABLE_HPP
