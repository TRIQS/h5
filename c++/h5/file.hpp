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

#pragma once
#include <vector>
#include "./object.hpp"

namespace h5 {

  /**
   *  A little handler for the HDF5 file
   *
   *  The class is basically a pointer to the file.
   */
  class file : public object {

    public:
    file() = default; // for python converter only

    /**
     * Open the file
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

    ///
    file(std::string const &name, char mode) : file(name.c_str(), mode) {}

    /// Name of the file
    [[nodiscard]] std::string name() const;

    /// Flush the file
    void flush();
  };

  /**
   * An hdf5 file in memory buffer
   */
  class memory_file : public file {

    public:
    /// A writable file in memory with a buffer
    memory_file();

    /// A read_only file on top on the buffer.
    memory_file(std::vector<unsigned char> const &buf);

    /// Get a copy of the buffer
    [[nodiscard]] std::vector<unsigned char> as_buffer() const;
  };

} // namespace h5
