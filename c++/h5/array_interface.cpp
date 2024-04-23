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
 * @brief Implementation details for array_interface.hpp.
 */

#include "./array_interface.hpp"
#include "./macros.hpp"
#include "./stl/string.hpp"

#include <hdf5.h>
#include <hdf5_hl.h>

#include <numeric>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace h5::array_interface {

  namespace {

    // Create an HDF5 memory dataspace.
    dataspace make_mem_dspace(array_view const &v) {
      // scalar case
      if (v.rank() == 0) return H5Screate(H5S_SCALAR);

      // create a dataspace of rank v.rank() and with shape v.parent_shape
      dataspace dspace = H5Screate_simple(v.slab.rank(), v.parent_shape.data(), nullptr);
      if (!dspace.is_valid()) throw std::runtime_error("Error in make_mem_dspace: Creating the dataspace for an array_view failed");

      // select the hyperslab according to v.slab
      herr_t err = H5Sselect_hyperslab(dspace, H5S_SELECT_SET, v.slab.offset.data(), v.slab.stride.data(), v.slab.count.data(),
                                       (v.slab.block.empty() ? nullptr : v.slab.block.data()));
      if (err < 0) throw std::runtime_error("Error in make_mem_dspace: Selecting the hyperslab failed");

      // return the dataspace
      return dspace;
    }

  } // namespace

  std::pair<v_t, v_t> get_parent_shape_and_h5_strides(long const *np_strides, int rank, long view_size) {
    // scalar case: return empty vectors
    if (rank == 0) return {};

    // empty view case: return (0,0,0), (1,1,1)
    if (view_size == 0) return {v_t(rank, 0), v_t(rank, 1)};

    // for the general case, we would like to find a parent_shape and h5_strides such that the following equations hold (rank = N):
    // 1. np_strides[N - 1] = h5_strides[N - 1]
    // 2. np_strides[N - 2] = h5_strides[N - 2] * parent_shape[N - 1]
    // 3. np_strides[N - 3] = h5_strides[N - 3] * parent_shape[N - 1] * parent_shape[N - 2]
    // ...
    // N. np_strides[N - N] = h5_strides[N - N] * parent_shape[N - 1] * parent_shape[N - 2] * ... * parent_shape[1]
    //
    // Note: The shape of the parent array as well as the HDF5 strides that fulfill the above equations are not unique.
    // This is not a problem as long as HDF5 manages to select the correct elements in memory.

    // create the result vectors
    v_t parent_shape(rank), h5_strides(rank);

    // from the above equations it follows that parent_shape[0] (size of the slowest varying dimension) is arbitrary
    parent_shape[0] = view_size;

    // We initialize the h5_strides array to the values of np_strides
    // and successively divide off the parent_shape values as they appear
    // in the equations above.
    for (int u = 0; u < rank; ++u) h5_strides[u] = np_strides[u];
    for (int u = rank - 2; u >= 0; --u) {
      hsize_t gcd = h5_strides[u];
      for (int v = u - 1; v >= 0; --v) { gcd = std::gcd(gcd, h5_strides[v]); }
      parent_shape[u + 1] = gcd;
      for (int v = u; v >= 0; --v) { h5_strides[v] /= gcd; }
    }

    return {parent_shape, h5_strides};
  }

  dataset_info get_dataset_info(dataset ds) {
    // retrieve shape and datatype information
    datatype ty                = H5Dget_type(ds);
    bool has_complex_attribute = H5LTfind_attribute(ds, "__complex__");
    dataspace dspace           = H5Dget_space(ds);
    int rank                   = H5Sget_simple_extent_ndims(dspace);
    v_t dims_out(rank);
    H5Sget_simple_extent_dims(dspace, dims_out.data(), nullptr);

    return {std::move(dims_out), ty, has_complex_attribute};
  }

  dataset_info get_dataset_info(group g, std::string const &name) {
    dataset ds = g.open_dataset(name);
    return get_dataset_info(ds);
  }

  void write(group g, std::string const &name, array_view const &v, bool compress) {
    // unlink the dataset if it already exists
    g.unlink(name);

    // shape of the hyperslab in memory
    auto hs_shape = v.slab.shape();

    // chunk the dataset and add compression
    proplist cparms = H5P_DEFAULT;
    if (compress and (v.rank() != 0)) {
      int n_dims = v.rank();
      std::vector<hsize_t> chunk_dims(n_dims);
      hsize_t const max_chunk_size = hsize_t{1UL << 32} - hsize_t{1}; // 2^32 - 1 = 4 GB
      hsize_t chunk_size           = H5Tget_size(v.ty);
      for (int i = v.rank() - 1; i >= 0; --i) {
        H5_ASSERT(max_chunk_size >= chunk_size);
        hsize_t max_dim = max_chunk_size / chunk_size;
        chunk_dims[i]   = std::clamp(hs_shape[i], hsize_t{1}, max_dim);
        chunk_size *= chunk_dims[i];
      }
      cparms = H5Pcreate(H5P_DATASET_CREATE);
      H5Pset_chunk(cparms, n_dims, chunk_dims.data());
      H5Pset_deflate(cparms, 1);
    }

    // dataspace for the dataset in the file
    dataspace file_dspace = H5Screate_simple(v.slab.rank(), hs_shape.data(), nullptr);

    // create the dataset in the file
    dataset ds = H5Dcreate2(g, name.c_str(), v.ty, file_dspace, H5P_DEFAULT, cparms, H5P_DEFAULT);
    if (!ds.is_valid())
      throw std::runtime_error("Error in h5::array_interface::write: Creating the dataset " + name + " in the group " + g.name() + " failed");

    // memory dataspace
    dataspace mem_dspace = make_mem_dspace(v);

    // write to the file dataset
    if (H5Sget_simple_extent_npoints(mem_dspace) > 0) { // avoid writing empty arrays
      herr_t err = H5Dwrite(ds, v.ty, mem_dspace, H5S_ALL, H5P_DEFAULT, v.start);
      if (err < 0)
        throw std::runtime_error("Error in h5::array_interface::write: Writing to the dataset " + name + " in the group" + g.name() + " failed");
    }

    // add complex attribute if the data is complex valued
    if (v.is_complex) h5_write_attribute(ds, "__complex__", "1");
  }

  void write_slice(group g, std::string const &name, array_view const &v, hyperslab sl) {
    // empty hyperslab
    if (sl.empty()) return;

    // check consistency of input
    if (v.slab.size() != sl.size()) throw std::runtime_error("Error in h5::array_interface::write_slice: Incompatible sizes");

    auto ds_info = get_dataset_info(g, name);
    if (not hdf5_type_equal(v.ty, ds_info.ty))
      throw std::runtime_error("Error in h5::array_interface::write_slice: Incompatible HDF5 types: " + get_name_of_h5_type(v.ty)
                               + " != " + get_name_of_h5_type(ds_info.ty));

    // open existing dataset, get dataspace and select hyperslab
    dataset ds            = g.open_dataset(name);
    dataspace file_dspace = H5Dget_space(ds);
    herr_t err            = H5Sselect_hyperslab(file_dspace, H5S_SELECT_SET, sl.offset.data(), sl.stride.data(), sl.count.data(),
                                                (sl.block.empty() ? nullptr : sl.block.data()));
    if (err < 0) throw std::runtime_error("Error in h5::array_interface::write_slice: Selecting the hyperslab failed");

    // memory dataspace
    dataspace mem_dspace = make_mem_dspace(v);

    // write to the selected hyperslab of the file dataset
    if (H5Sget_simple_extent_npoints(file_dspace) > 0) {
      err = H5Dwrite(ds, v.ty, mem_dspace, file_dspace, H5P_DEFAULT, v.start);
      if (err < 0)
        throw std::runtime_error("Error in h5::array_interface::write_slice: Writing the dataset " + name + " in the group " + g.name() + " failed");
    }
  }

  void write_attribute(object obj, std::string const &name, array_view v) {
    // check if the attribute already exists
    if (H5LTfind_attribute(obj, name.c_str()) != 0)
      throw std::runtime_error("Error in h5::array_interface::write_attribute: Attribute " + name + " already exists");

    // memory dataspace
    dataspace mem_dspace = make_mem_dspace(v);

    // create attribute to write to
    attribute attr = H5Acreate2(obj, name.c_str(), v.ty, mem_dspace, H5P_DEFAULT, H5P_DEFAULT);
    if (!attr.is_valid()) throw std::runtime_error("Error in h5::array_interface::write_attribute: Creating the attribute " + name + " failed");

    // write to the attribute
    herr_t err = H5Awrite(attr, v.ty, v.start);
    if (err < 0) throw std::runtime_error("Error in h5::array_interface::write_attribute: Writing to the attribute " + name + " failed");
  }

  void read(group g, std::string const &name, array_view v, hyperslab sl) {
    // open dataset and get dataspace
    dataset ds            = g.open_dataset(name);
    dataspace file_dspace = H5Dget_space(ds);

    // if provided, select the hyperslab of the file dataspace
    if (not sl.empty()) {
      herr_t err = H5Sselect_hyperslab(file_dspace, H5S_SELECT_SET, sl.offset.data(), sl.stride.data(), sl.count.data(),
                                       (sl.block.empty() ? nullptr : sl.block.data()));
      if (err < 0) throw std::runtime_error("Error in h5::array_interface::read: selecting the hyperslab failed");
    }

    // check consistency of input
    auto ds_info = get_dataset_info(g, name);
    if (H5Tget_class(v.ty) != H5Tget_class(ds_info.ty))
      throw std::runtime_error("Error in h5::array_interface::read: Incompatible HDF5 types: " + get_name_of_h5_type(v.ty)
                               + " != " + get_name_of_h5_type(ds_info.ty));

    if (not hdf5_type_equal(v.ty, ds_info.ty))
      std::cerr << "WARNING: HDF5 type mismatch while reading into an array_view: " + get_name_of_h5_type(v.ty)
            + " != " + get_name_of_h5_type(ds_info.ty) + "\n";

    auto sl_size = sl.size();
    if (sl.empty()) { sl_size = std::accumulate(ds_info.lengths.begin(), ds_info.lengths.end(), (hsize_t)1, std::multiplies<>()); }
    if (sl_size != v.slab.size()) throw std::runtime_error("Error in h5::array_interface::read: Incompatible sizes");

    // memory dataspace
    dataspace mem_dspace = make_mem_dspace(v);

    // read the selected hyperslab from the file dataset
    if (H5Sget_simple_extent_npoints(file_dspace) > 0) {
      herr_t err = H5Dread(ds, v.ty, mem_dspace, file_dspace, H5P_DEFAULT, v.start);
      if (err < 0)
        throw std::runtime_error("Error in h5::array_interface::read: Reading the dataset " + name + " in the group " + g.name() + " failed");
    }
  }

  void read_attribute(object obj, std::string const &name, array_view v) {
    // open attribute
    attribute attr = H5Aopen(obj, name.c_str(), H5P_DEFAULT);
    if (!attr.is_valid()) throw std::runtime_error("Error in h5::array_interface::read_attribute: Opening the attribute " + name + " failed");

    // get dataspace information
    dataspace space = H5Aget_space(attr);
    int rank        = H5Sget_simple_extent_ndims(space);
    if (rank != 0) throw std::runtime_error("Error in h5::array_interface::read_attribute: Attribute " + name + " has a rank != 0");

    // get datatype information
    auto eq = H5Tequal(H5Aget_type(attr), v.ty);
    if (eq < 0) throw std::runtime_error("Error in h5::array_interface::read_attribute: H5Tequal call failed");
    if (eq == 0) throw std::runtime_error("Error in h5::array_interface::read_attribute: Incompatible HDF5 types");

    // read the attribute
    auto err = H5Aread(attr, v.ty, v.start);
    if (err < 0) throw std::runtime_error("Error in h5::array_interface::read_attribute: Reading the attribute " + name + " failed");
  }

} // namespace h5::array_interface
