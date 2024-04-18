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
//
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Implementation details for stl/string.hpp.
 */

#include "./string.hpp"
#include "../macros.hpp"
#include "../utils.hpp"

#include <hdf5.h>
#include <hdf5_hl.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace h5 {

  namespace {

    // Returns an HDF5 datatype for a fixed-sized string with the given size or a variable-sized
    // string if size == H5T_VARIABLE.
    datatype str_dtype(size_t size = H5T_VARIABLE) {
      datatype dt = H5Tcopy(H5T_C_S1);
      auto err    = H5Tset_size(dt, size);
      H5Tset_cset(dt, H5T_CSET_UTF8);
      if (err < 0) throw std::runtime_error("Error in str_dtype: H5Tset_size call failed");
      return dt;
    }

  } // namespace

  void h5_write(group g, std::string const &name, std::string const &s) {
    // create the dataset for a variable-sized string
    datatype dt     = str_dtype();
    dataspace space = H5Screate(H5S_SCALAR);
    dataset ds      = g.create_dataset(name, dt, space);

    // write the string to dataset
    auto *s_ptr = s.c_str();
    auto err    = H5Dwrite(ds, dt, H5S_ALL, H5S_ALL, H5P_DEFAULT, &s_ptr);
    if (err < 0) throw std::runtime_error("Error in h5_write: Writing a string to the dataset " + name + " in the group " + g.name() + " failed");
  }

  void h5_read(group g, std::string const &name, std::string &s) {
    // clear the string
    s = "";

    // open the dataset and get dataspace and datatype information
    dataset ds       = g.open_dataset(name);
    dataspace dspace = H5Dget_space(ds);
    int rank         = H5Sget_simple_extent_ndims(dspace);
    if (rank != 0) throw std::runtime_error("Error in h5_read: Reading a string from a dataspace with rank != 0 is not possible");

    datatype dt = H5Dget_type(ds);
    H5_ASSERT(H5Tget_class(dt) == H5T_STRING);

    // variable-sized string
    if (H5Tis_variable_str(dt)) {
      // first read into a char* pointer, then copy into the string
      std::array<char *, 1> rd_ptr{nullptr};
      auto err = H5Dread(ds, dt, H5S_ALL, H5S_ALL, H5P_DEFAULT, rd_ptr.data());
      if (err < 0) throw std::runtime_error("Error in h5_read: Reading a string from the dataset " + name + " in the group " + g.name() + " failed");
      s.append(rd_ptr[0]);

      // free the resources allocated in the variable-length read
      err = H5Dvlen_reclaim(dt, dspace, H5P_DEFAULT, rd_ptr.data());
      if (err < 0) throw std::runtime_error("Error in h5_read: Freeing resources after reading a variable-length string failed");
    } else { // fixed-sized string
      std::vector<char> buf(H5Tget_size(dt) + 1, 0x00);
      auto err = H5Dread(ds, dt, H5S_ALL, H5S_ALL, H5P_DEFAULT, &buf[0]);
      if (err < 0) throw std::runtime_error("Error in h5_read: Reading a string from the dataset " + name + " in the group " + g.name() + " failed");
      s.append(&buf.front());
    }
  }

  void h5_write_attribute(object obj, std::string const &name, std::string const &s) {
    // create the variable-sized string datatype and the dataspace
    datatype dt     = str_dtype();
    dataspace space = H5Screate(H5S_SCALAR);

    // create the attribute
    attribute attr = H5Acreate2(obj, name.c_str(), dt, space, H5P_DEFAULT, H5P_DEFAULT);
    if (!attr.is_valid()) throw std::runtime_error("Error in h5_write_attribute: Creating the attribute " + name + " failed");

    // write the string to attribute
    auto *s_ptr = s.c_str();
    herr_t err  = H5Awrite(attr, dt, &s_ptr);
    if (err < 0) throw std::runtime_error("Error in h5_write_attribute: Writing a string to the attribute " + name + " failed");
  }

  void h5_read_attribute(object obj, std::string const &name, std::string &s) {
    // clear the string and return if the attribute is not present
    s = "";
    if (H5LTfind_attribute(obj, name.c_str()) == 0) return;

    // open the attribute and get dataspace and datatype information
    attribute attr   = H5Aopen(obj, name.c_str(), H5P_DEFAULT);
    dataspace dspace = H5Aget_space(attr);
    int rank         = H5Sget_simple_extent_ndims(dspace);
    if (rank != 0) throw std::runtime_error("Error in h5_read_attribute: Reading a string from a dataspace with rank != 0 is not possible");

    datatype dt = H5Aget_type(attr);
    H5_ASSERT(H5Tget_class(dt) == H5T_STRING);

    // variable-sized string
    if (H5Tis_variable_str(dt)) {
      // first read into a char* pointer, then copy into the string
      std::array<char *, 1> rd_ptr{nullptr};
      auto err = H5Aread(attr, dt, rd_ptr.data());
      if (err < 0) throw std::runtime_error("Error in h5_read_attribute: Reading a string from the attribute " + name + " failed");
      s.append(rd_ptr[0]);

      // free the resources allocated in the variable-length read
      err = H5Dvlen_reclaim(dt, dspace, H5P_DEFAULT, rd_ptr.data());
      if (err < 0) throw std::runtime_error("Error in h5_read_attribute: Freeing resources after reading a variable-length string failed");
    } else { // fixed-sized string
      std::vector<char> buf(H5Tget_size(dt) + 1, 0x00);
      auto err = H5Aread(attr, dt, (void *)(&buf[0]));
      if (err < 0) throw std::runtime_error("Error in h5_read_attribute: Reading a string from the attribute " + name + " failed");
      s.append(&buf.front());
    }
  }

  void h5_write_attribute_to_key(group g, std::string const &key, std::string const &name, std::string const &s) {
    // create the variable-sized string datatype and dataspace
    datatype dt      = str_dtype();
    dataspace dspace = H5Screate(H5S_SCALAR);

    // create the attribute for a given key
    attribute attr = H5Acreate_by_name(g, key.c_str(), name.c_str(), dt, dspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (!attr.is_valid()) throw std::runtime_error("Error in h5_write_attribute_to_key: Creating the attribute " + name + " failed");

    // write the string to the attribute
    herr_t err = H5Awrite(attr, dt, (void *)(s.c_str()));
    if (err < 0) throw std::runtime_error("Error in h5_write_attribute_to_key: Writing a string to the attribute " + name + " failed");
  }

  void h5_read_attribute_from_key(group g, std::string const &key, std::string const &name, std::string &s) {
    // clear the string and return if the attribute is not present
    s = "";
    if (H5Aexists_by_name(g, key.c_str(), name.c_str(), H5P_DEFAULT) == 0) return;

    // open the attribute and get dataspace and datatype information
    attribute attr   = H5Aopen_by_name(g, key.c_str(), name.c_str(), H5P_DEFAULT, H5P_DEFAULT);
    dataspace dspace = H5Aget_space(attr);
    int rank         = H5Sget_simple_extent_ndims(dspace);
    if (rank != 0) throw std::runtime_error("Error in h5_read_attribute_to_key: Reading a string from a dataspace with rank != 0 is not possible");

    datatype dt = H5Aget_type(attr);
    H5_ASSERT(H5Tget_class(dt) == H5T_STRING);

    // variable-sized string
    if (H5Tis_variable_str(dt)) {
      // first read into a char* pointer, then copy into the string
      std::array<char *, 1> rd_ptr{nullptr};
      auto err = H5Aread(attr, dt, rd_ptr.data());
      if (err < 0) throw std::runtime_error("Error in h5_read_attribute_to_key: Reading a string from the attribute " + name + " failed");
      s.append(rd_ptr[0]);

      // free the resources allocated in the variable-length read
      err = H5Dvlen_reclaim(dt, dspace, H5P_DEFAULT, rd_ptr.data());
      if (err < 0) throw std::runtime_error("Error in h5_read_attribute_to_key: Rreeing resources after reading a variable-length string failed");
    } else { // fixed-sized string
      std::vector<char> buf(H5Tget_size(dt) + 1, 0x00);
      auto err = H5Aread(attr, dt, &buf[0]);
      if (err < 0) throw std::runtime_error("Error in h5_read_attribute_to_key: Reading a string from the attribute " + name + " failed");
      s.append(&buf.front());
    }
  }

  // HDF5 datatype of a char_buf is a fixed-sized string
  datatype char_buf::dtype() const { return str_dtype(lengths.back()); }

  // dataspace is an n-dimensional array of fixed-sized strings, each of length max_length + 1
  dataspace char_buf::dspace() const {
    dataspace ds = H5Screate_simple(static_cast<int>(lengths.size()) - 1, lengths.data(), nullptr);
    if (!ds.is_valid()) throw make_runtime_error("Error in h5::char_buf: Creating the dataspace for the char_buf failed");
    return ds;
  }

  void h5_write(group g, std::string const &name, char_buf const &cb) {
    // create the dataset for the char_buf
    auto dt     = cb.dtype();
    auto dspace = cb.dspace();
    dataset ds  = g.create_dataset(name, dt, dspace);

    // write to the dataset
    auto err = H5Dwrite(ds, dt, dspace, H5S_ALL, H5P_DEFAULT, (void *)cb.buffer.data());
    if (err < 0) throw make_runtime_error("Error in h5_write: Writing a char_buf to the dataset ", name, " in the group ", g.name(), " failed");
  }

  void h5_read(group g, std::string const &name, char_buf &cb) {
    // open the dataset and get dataspace and datatype information
    dataset ds       = g.open_dataset(name);
    dataspace dspace = H5Dget_space(ds);
    datatype ty      = H5Dget_type(ds);

    // prepare the char_buf to be read into
    char_buf cb_out;
    // number of strings
    int dim = H5Sget_simple_extent_ndims(dspace);
    cb_out.lengths.resize(dim);
    H5Sget_simple_extent_dims(dspace, cb_out.lengths.data(), nullptr);
    // max. length of the strings + 1
    size_t size = H5Tget_size(ty);
    cb_out.lengths.push_back(size);
    // resize the buffer
    long ltot = std::accumulate(cb_out.lengths.begin(), cb_out.lengths.end(), 1, std::multiplies<>());
    cb_out.buffer.resize(std::max(ltot, 1l), 0x00);

    // read into the buffer
    H5_ASSERT(hdf5_type_equal(ty, cb_out.dtype()));
    auto err = H5Dread(ds, ty, cb_out.dspace(), H5S_ALL, H5P_DEFAULT, (void *)cb_out.buffer.data());
    if (err < 0) throw make_runtime_error("Error in h5_read: Reading a char_buf from the dataset ", name, " in the group ", g.name(), " failed");

    // move to output char_buf
    cb = std::move(cb_out);
  }

  void h5_write_attribute(object obj, std::string const &name, char_buf const &cb) {
    // datatype and dataspace of char_buf
    auto dt     = cb.dtype();
    auto dspace = cb.dspace();

    // create the attribute
    attribute attr = H5Acreate2(obj, name.c_str(), dt, dspace, H5P_DEFAULT, H5P_DEFAULT);
    if (!attr.is_valid()) throw make_runtime_error("Error in h5_write_attribute: Creating the attribute ", name, " failed");

    // write the char_buf to the attribute
    herr_t status = H5Awrite(attr, dt, (void *)cb.buffer.data());
    if (status < 0) throw make_runtime_error("Error in h5_write_attribute: Writing a char_buf to the attribute ", name, " failed");
  }

  void h5_read_attribute(object obj, std::string const &name, char_buf &cb) {
    // open the attribute and get dataspace and datatype information
    attribute attr = H5Aopen(obj, name.c_str(), H5P_DEFAULT);
    if (!attr.is_valid()) throw make_runtime_error("Error in h5_read_attribute: Opening the attribute ", name, " failed");

    dataspace d_space = H5Aget_space(attr);
    datatype ty       = H5Aget_type(attr);

    // prepare the char_buf to be read into
    char_buf cb_out;
    // number of strings
    int dim = H5Sget_simple_extent_ndims(d_space);
    cb_out.lengths.resize(dim);
    H5Sget_simple_extent_dims(d_space, cb_out.lengths.data(), nullptr);
    // max. length of the strings + 1
    size_t size = H5Tget_size(ty);
    cb_out.lengths.push_back(size);
    // resize the buffer
    long ltot = std::accumulate(cb_out.lengths.begin(), cb_out.lengths.end(), 1, std::multiplies<>());
    cb_out.buffer.resize(std::max(ltot, 1l), 0x00);

    // read into the buffer
    H5_ASSERT(hdf5_type_equal(ty, cb_out.dtype()));
    auto err = H5Aread(attr, ty, (void *)cb_out.buffer.data());
    if (err < 0) throw make_runtime_error("Error in h5_read_attribute: Reading a char_buf from the attribute ", name, " failed");

    // move to output char_buf
    cb = std::move(cb_out);
  }

} // namespace h5
