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
#include <vector>

using namespace std::string_literals;

#define CHECK_OR_THROW(Cond, Mess)                                                                                                                   \
  if (!(Cond)) throw std::runtime_error("Error in h5 interface : "s + Mess);

namespace h5 {

  file::file(const char *name, char mode) {
    switch (mode) {
      case 'r': id = H5Fopen(name, H5F_ACC_RDONLY, H5P_DEFAULT); break;
      case 'w': id = H5Fcreate(name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT); break;
      case 'a':
        // Turn off error handling
        herr_t (*old_func)(void *);
        void *old_client_data;
        H5Eget_auto1(&old_func, &old_client_data);
        H5Eset_auto1(nullptr, nullptr);

        // This may fail
        id = H5Fcreate(name, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);

        // Turn on error handling
        H5Eset_auto1(old_func, old_client_data);

        // Open in RDWR if creation failed
        if (id < 0) id = H5Fopen(name, H5F_ACC_RDWR, H5P_DEFAULT);
        break;
      case 'e': id = H5Fopen(name, H5F_ACC_EXCL, H5P_DEFAULT); break;
      default: throw std::runtime_error("HDF5 file opening : mode is not r, w, a, e. Cf documentation");
    }

    if (id < 0) throw std::runtime_error("HDF5 : cannot "s + (((mode == 'r') or (mode == 'a')) ? "open" : "create") + "file : "s + name);
  }

  //---------------------------------------------

  std::string file::name() const { // same function as for group
    char _n[1];
    ssize_t size = H5Fget_name(id, _n, 1); // first call, get the size only
    std::vector<char> buf(size + 1, 0x00);
    H5Fget_name(id, buf.data(), size + 1); // now get the name
    std::string res = "";
    res.append(&(buf.front()));
    return res;
  }

  //---------------------------------------------

  void file::flush() {
    if (not is_valid()) return;
    auto err = H5Fflush(id, H5F_SCOPE_GLOBAL);
    CHECK_OR_THROW((err >= 0), "flushing the file");
  }

  // -------------------------

  file::file() {

    proplist fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK_OR_THROW((fapl >= 0), "creating fapl");

    auto err = H5Pset_fapl_core(fapl, (size_t)(64 * 1024), false);
    CHECK_OR_THROW((err >= 0), "setting core file driver in fapl.");

    this->id = H5Fcreate("MemoryBuffer", 0, H5P_DEFAULT, fapl);
    CHECK_OR_THROW((this->is_valid()), "created core file");
  }

  // -------------------------

  file::file(const std::byte *buf, size_t size) {

    proplist fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK_OR_THROW((fapl >= 0), "creating fapl");

    auto err = H5Pset_fapl_core(fapl, (size_t)(64 * 1024), false);
    CHECK_OR_THROW((err >= 0), "setting core file driver in fapl.");

    err = H5Pset_file_image(fapl, (void *)buf, size);
    CHECK_OR_THROW((err >= 0), "set file image in fapl.");

    this->id = H5Fopen("MemoryBuffer", H5F_ACC_RDWR, fapl);
    CHECK_OR_THROW((this->is_valid()), "opened received file image file");
  }

  // -------------------------

  std::vector<std::byte> file::as_buffer() const {

    auto f   = hid_t(*this);
    auto err = H5Fflush(f, H5F_SCOPE_GLOBAL);
    CHECK_OR_THROW((err >= 0), "flushed core file.");

    ssize_t image_len = H5Fget_file_image(f, nullptr, (size_t)0);
    CHECK_OR_THROW((image_len > 0), "got image file size");

    std::vector<std::byte> buf(image_len, std::byte{0});

    ssize_t bytes_read = H5Fget_file_image(f, (void *)buf.data(), (size_t)image_len);
    CHECK_OR_THROW(bytes_read == image_len, "wrote file into image buffer");

    return buf;
  }

} // namespace h5
