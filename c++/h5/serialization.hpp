// Copyright (c) 2019-2021 Simons Foundation
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
 * @brief Provides generic serialize and deserialize functions for types that can be read/written from/to HDF5.
 */

#ifndef LIBH5_SERIALIZATION_HPP
#define LIBH5_SERIALIZATION_HPP

#include "./file.hpp"
#include "./generic.hpp"

#include <cstddef>
#include <vector>

namespace h5 {

  /**
   * @addtogroup serialize
   * @{
   */

  /**
   * @brief Serialize an object to a byte buffer.
   *
   * @details It first writes the object to a buffered memory file and then returns the underlying byte buffer.
   *
   * @tparam T Type of the object.
   * @param x Object to be serialized.
   * @return Byte buffer containing the serialized object.
   */
  template <typename T>
  [[nodiscard]] std::vector<std::byte> serialize(T const &x) {
    file f{};
    h5_write(f, "object", x);
    return f.as_buffer();
  }

  /**
   * @brief Deserialize an object from a byte buffer.
   *
   * @details It first creates a buffered memory file from the given byte buffer and then reads the object from the file.
   *
   * @tparam T Type of the object.
   * @param buf Byte buffer containing the serialized object.
   * @return Object restored from the given byte buffer.
   */
  template <typename T>
  [[nodiscard]] T deserialize(std::vector<std::byte> const &buf) {
    file f{buf};
    return h5_read<T>(f, "object");
  }

  /** @} */

} // namespace h5

#endif // LIBH5_SERIALIZATION_HPP
