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

#ifndef LIBH5_FILE_HPP
#define LIBH5_FILE_HPP

#include <vector>
#include <span>
#include "./object.hpp"

namespace h5 {

  /**
   *  A little handler for the HDF5 file
   *
   *  The class is basically a pointer to the file.
   */
  class file : public object {

    public:
    /**
     * Open a file in memory
     */
    file();

    /**
     * Open the file on disk
     *
     * @param name  name of the file
     *
     * @param mode  Opening mode
     * 
     *       - 'r' : Read Only (HDF5 flag H5F_ACC_RDONLY)
     *       - 'w' : Write Only (HDF5 flag H5F_ACC_TRUNC)
     *       - 'a' : Append (HDF5 flag  H5F_ACC_RDWR)
     *       - 'e' : Like 'w' but fails if the file already exists (HDF5 flag  H5F_ACC_EXCL)
     */
    file(const char *name, char mode);

    // Open the file on disk
    file(std::string const &name, char mode) : file(name.c_str(), mode) {}

    /// Name of the file
    [[nodiscard]] std::string name() const;

    /// Flush the file
    void flush();

    private:
    file(const std::byte *buf, size_t size);

    public:
    /// Create a file in memory from a byte buffer
    file(std::span<std::byte> const &buf) : file(buf.data(), buf.size()) {}

    /// Create a file in memory from a byte buffer
    file(std::vector<std::byte> const &buf) : file(buf.data(), buf.size()) {}

    /// Get a copy of the associated byte buffer
    [[nodiscard]] std::vector<std::byte> as_buffer() const;
  };

} // namespace h5

#endif // LIBH5_FILE_HPP
