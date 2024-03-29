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

#ifndef LIBH5_ARRAY_INTERFACE_HPP
#define LIBH5_ARRAY_INTERFACE_HPP

#include <utility>
#include <vector>
#include <string>
#include "./group.hpp"

// This is the generic interface for the ndarray interface
//
namespace h5::array_interface {

  // Stores the hdf5 type and the dims
  struct h5_lengths_type {
    v_t lengths;
    datatype ty;
    bool has_complex_attribute;

    //
    [[nodiscard]] int rank() const { return lengths.size(); }
  };

  // Store HDF5 hyperslab info, as in HDF5 manual
  // http://davis.lbl.gov/Manuals/HDF5-1.8.7/UG/12_Dataspaces.html
  struct hyperslab {
    v_t offset; // index offset for each dimension
    v_t stride; // stride in each dimension (in the HDF5 sense). 1 if contiguous. Always >0.
    v_t count;  // length in each dimension
    v_t block;  // block in each dimension

    // Constructor : unique to enforce the proper sizes of array
    // rank : the array will have rank + is_complex
    hyperslab(int rank, bool is_complex)
       : offset(rank + is_complex, 0),
         stride(rank + is_complex, 1),
         count(rank + is_complex, 0),
         block(rank + is_complex, 1) { // block is often unused
      if (is_complex) {
        stride[rank] = 1;
        count[rank]  = 2;
      }
    }

    hyperslab() = default;

    //
    [[nodiscard]] int rank() const { return count.size(); }
    [[nodiscard]] bool empty() const { return count.empty(); }
  };

  // Stores a view of an array.
  // scalar are array of rank 0, lengths, strides are empty, rank is 0, start is the scalar
  struct h5_array_view {
    datatype ty;    // HDF5 type
    void *start;    // start of data. It MUST be a pointer of T* with ty = hdf5_type<T>()
    v_t L_tot;      // lengths of the parent contiguous array
    hyperslab slab; // hyperslab
    bool is_complex;

    // Constructor : unique to enforce the proper sizes of array
    h5_array_view(datatype ty, void *start, int rank, bool is_complex)
       : ty(std::move(ty)), start(start), L_tot(rank + is_complex), slab(rank, is_complex), is_complex(is_complex) {
      if (is_complex) L_tot[rank] = 2;
    }

    [[nodiscard]] int rank() const { return slab.rank(); }
  };

  //------------------------------------------------
  // Given the array strides, rank and size return the total dimensions L_tot and the h5-strides
  // Assume stride_order is C.
  // use strides[rank -1]  =   strides_h5 [rank -1]
  //     strides[rank -2]  =   L[rank-1] * strides_h5[rank -2]
  //     strides[rank -3]  =   L[rank-1] * L[rank-2] * strides_h5[rank -3]
  //     strides[0]        =   L[rank-1] * L[rank-2] * L[1] * strides_h5[0]
  std::pair<v_t, v_t> get_L_tot_and_strides_h5(long const *stri, int rank, long total_size);

  // Retrieve lengths and hdf5 type from a dataset g[name] or attribute obj[name]
  h5_lengths_type get_h5_lengths_type(group g, std::string const &name);

  // Write the view of the array to the group
  void write(group g, std::string const &name, h5_array_view const &a, bool compress);

  // Write to part of an existing dataset in file
  void write_slice(group g, std::string const &name, h5_array_view const &a, h5_lengths_type lt, hyperslab sl);

  // Read into an array_view from the group
  void read(group g, std::string const &name, h5_array_view v, h5_lengths_type lt, hyperslab sl = {});

  // Write the view of the array to the attribute
  void write_attribute(object obj, std::string const &name, h5_array_view v);

  // Read into a contiguous array_view from the attribute
  void read_attribute(object obj, std::string const &name, h5_array_view v);

} // namespace h5::array_interface

#endif // LIBH5_ARRAY_INTERFACE_HPP
