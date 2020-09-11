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

#ifndef LIBH5_FILE_HPP
#define LIBH5_FILE_HPP

#include <vector>
#include "./object.hpp"

struct _object;           // forward
typedef _object PyObject; // forward

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

    size_t get_size() const;
    void write_to_buffer(unsigned char *buf_ptr, ssize_t buf_size) const;
    memory_file(unsigned char const *buf_ptr, ssize_t buf_size);

    public:
    /// A writable file in memory with a buffer
    memory_file();

    /// A read_only file on top on the buffer.
    memory_file(std::vector<unsigned char> const &buf);

    /// Get a copy of the buffer
    [[nodiscard]] std::vector<unsigned char> as_buffer() const;

    /// A read_only file on top on the python bytearray. NB : implemented in the hdf5_io library.
    memory_file(PyObject *py_byte_array);

    /// Get a PyByteArray from the memory file. Returns a new reference. NB : implemented in the hdf5_io library.
    [[nodiscard]] PyObject *as_py_byte_array() const;
  };

  // -----------------------------
  // NB : I don't need Python until I actually instantiate these templates
  // in a module ...
  // but it is better to have them included in the h5.hpp 
  // FIXME : separate ? 

  template <typename T>
  PyObject *serialize_to_pybytearray(T const &x) {
    memory_file f;
    h5_write(f, "object", x);
    return f.as_py_byte_array();
  }

  // -----------------------------

  template <typename T>
  T deserialize_from_pybytearray(PyObject *py_byte_array) {
    memory_file f{py_byte_array};
    return h5_read<T>(f, "object");
  }

} // namespace h5

#endif // LIBH5_FILE_HPP
