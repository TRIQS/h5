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

#include "./file.hpp"

#include <hdf5.h>
#include <hdf5_hl.h>

#include <stdexcept>

using namespace std::string_literals;

// throw an exception with a given message if a condition is not met
#define CHECK_OR_THROW(Cond, Mess)                                                                                                                   \
  if (!(Cond)) throw std::runtime_error("Error in h5::file: "s + (Mess));

namespace h5 {

  file::file(const char *name, char mode) {
    switch (mode) {
      // open existing file in read only mode
      case 'r': id = H5Fopen(name, H5F_ACC_RDONLY, H5P_DEFAULT); break;
      // create new or overwrite existing file in read-write mode
      case 'w': id = H5Fcreate(name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT); break;
      // create new or append to exisiting file in read-write mode
      case 'a': {
        // turn off error handling
        herr_t (*old_func)(void *) = nullptr;
        void *old_client_data      = nullptr;
        H5Eget_auto1(&old_func, &old_client_data);
        H5Eset_auto1(nullptr, nullptr);

        // this may fail
        id = H5Fcreate(name, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);

        // turn on error handling
        H5Eset_auto1(old_func, old_client_data);

        // open in read-write mode if creation failed
        if (id < 0) id = H5Fopen(name, H5F_ACC_RDWR, H5P_DEFAULT);
        break;
      }
      // create new file in read-write mode if the file does not exist yet
      case 'e': id = H5Fopen(name, H5F_ACC_EXCL, H5P_DEFAULT); break;
      default: throw std::runtime_error("File mode is not one of r, w, a, e");
    }

    // throw an exception if opening/creating the file failed
    if (id < 0) throw std::runtime_error("Opening/Creating the file "s + name + " failed");
  }

  // same function is used in h5::group
  std::string file::name() const {
    // get length of the name
    ssize_t size = H5Fget_name(id, nullptr, 1);

    // reserve a buffer and get name
    std::vector<char> buf(size + 1, 0x00);
    H5Fget_name(id, buf.data(), size + 1);

    // return string
    std::string res = "";
    res.append(&(buf.front()));
    return res;
  }

  void file::flush() {
    if (not is_valid()) return;
    auto err = H5Fflush(id, H5F_SCOPE_GLOBAL);
    CHECK_OR_THROW((err >= 0), "Flushing the file failed");
  }

  file::file() {
    // create a file access property list
    proplist fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK_OR_THROW((fapl >= 0), "Creating the fapl failed");

    // set the file driver to use the `H5FD_CORE` driver
    auto err = H5Pset_fapl_core(fapl, (size_t)(64 * 1024), false);
    CHECK_OR_THROW((err >= 0), "Setting the core file driver in fapl failed");

    // create a buffered memory file
    this->id = H5Fcreate("MemoryBuffer", 0, H5P_DEFAULT, fapl);
    CHECK_OR_THROW((this->is_valid()), "Creating a buffered memory file failed");
  }

  file::file(const std::byte *buf, size_t size) {
    // create a file access property list
    proplist fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK_OR_THROW((fapl >= 0), "Creating the fapl failed");

    // set the file driver to use the `H5FD_CORE` driver
    auto err = H5Pset_fapl_core(fapl, (size_t)(64 * 1024), false);
    CHECK_OR_THROW((err >= 0), "Setting the core file driver in fapl failed");

    // set the initial file image to the given memory buffer
    err = H5Pset_file_image(fapl, (void *)buf, size);
    CHECK_OR_THROW((err >= 0), "Setting the file image to a given memory buffer failed");

    // create a buffered memory file
    this->id = H5Fopen("MemoryBuffer", H5F_ACC_RDWR, fapl);
    CHECK_OR_THROW((this->is_valid()), "Creating a buffered memory file failed");
  }

  std::vector<std::byte> file::as_buffer() const {
    // flush the file
    auto f   = hid_t(*this);
    auto err = H5Fflush(f, H5F_SCOPE_GLOBAL);
    CHECK_OR_THROW((err >= 0), "Flushing the buffered memory file failed");

    // retrieve size of the file image
    ssize_t image_len = H5Fget_file_image(f, nullptr, (size_t)0);
    CHECK_OR_THROW((image_len > 0), "Getting the file image size failed");

    // create buffer and copy file image to it
    std::vector<std::byte> buf(image_len, std::byte{0});
    ssize_t bytes_read = H5Fget_file_image(f, (void *)buf.data(), (size_t)image_len);
    CHECK_OR_THROW(bytes_read == image_len, "Writing file image to buffer failed");

    return buf;
  }

} // namespace h5
