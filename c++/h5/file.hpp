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
 * @brief Provides a handle to an HDF5 file.
 */

#ifndef LIBH5_FILE_HPP
#define LIBH5_FILE_HPP

#include "./object.hpp"

#include <cstddef>
#include <span>
#include <string>
#include <vector>

namespace h5 {

  /**
   * @brief A handle to an HDF5 file.
   *
   * @details This class inherits from the general h5::object class. It simply wraps the HDF5 functions
   * to open an existing file or to create a new file.
   *
   * An h5::file is automatically closed when it goes out of scope, i.e. its reference count is decreased.
   */
  class file : public object {
    public:
    /**
     * @brief Default constructor creates a buffered memory file.
     * @details It modifies the file access property list to use the `H5FD_CORE` driver. It can be used for
     * serializing and deserializing data (see h5::serialize and h5::deserialize).
     */
    file();

    /**
     * @brief Constructor to open an existing file or to create a new file on disk.
     *
     * @details The file is opened in the specified mode. The following modes are available:
     *
     * - 'r': Open an existing file in read only mode (calls `H5Fopen` with `H5F_ACC_RDONLY`).
     * - 'w': Create a new file or overwrite an existing file in read-write mode (calls `H5Fcreate` with `H5F_ACC_TRUNC`).
     * - 'a': Create a new file or append to an existing file in read-write mode (calls `H5Fcreate` with  `H5F_ACC_EXCL`
     * or `H5Fopen` with `H5F_ACC_RDWR` in case the file already exists).
     * - 'e': Create a new file if the file does not already exists, otherwise throw an exception (calls `H5Fcreate` with
     * `H5F_ACC_EXCL`)
     *
     * @param name Name of the file.
     * @param mode Mode in which to open the file.
     */
    file(const char *name, char mode);

    /**
     * @brief Constructor to open an existing file or to create a new file on disk.
     * @details See file::file(const char*, char) for a more detailed description.
     */
    file(std::string const &name, char mode) : file(name.c_str(), mode) {}

    /// Get the name of the file.
    [[nodiscard]] std::string name() const;

    /// Flush the file by calling `H5Fflush`.
    void flush();

    private:
    // Constructor to create a buffered memory file with an initial file image of a given size.
    file(const std::byte *buf, size_t size);

    public:
    /**
     * @brief Constructor to create a buffered memory file from a byte buffer.
     * @param buf Byte buffer.
     */
    file(std::span<std::byte> const &buf) : file(buf.data(), buf.size()) {}

    /**
     * @brief Constructor to create a buffered memory file from a byte buffer.
     * @param buf Byte buffer.
     */
    file(std::vector<std::byte> const &buf) : file(buf.data(), buf.size()) {}

    /// Get a copy of the associated byte buffer.
    [[nodiscard]] std::vector<std::byte> as_buffer() const;
  };

} // namespace h5

#endif // LIBH5_FILE_HPP
