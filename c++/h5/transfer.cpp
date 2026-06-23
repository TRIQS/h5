// Copyright (c) 2019-2024 Simons Foundation
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
// Authors: Lukas Weber

/**
 * @file
 * @brief Implementation details for transfer.hpp.
 */

#include "./transfer.hpp"
#include "./object.hpp"

#include <hdf5.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace h5 {

  // anonymous namespace for internal functions
  namespace {

    // The built-in default conversion callback: warn for the h5-specific mismatch situations and stay silent
    // (unhandled) for HDF5's native per-element exceptions, preserving the historical behavior.
    conv_ret default_type_conv_cb(conv_except except, datatype src, datatype dst, void * /*src_buf*/, void * /*dst_buf*/) {
      if (except == conv_except::not_equal or except == conv_except::narrower) {
        std::cerr << "WARNING: HDF5 type mismatch, converting " + get_name_of_h5_type(src) + " -> " + get_name_of_h5_type(dst)
              + (except == conv_except::narrower ? " (narrowing)" : "") + "\n";
      }
      return conv_ret::unhandled;
    }

    // Storage for the library-global conversion callback (lazily initialized to the default).
    type_conv_cb_t &cb_ref() {
      static type_conv_cb_t cb = default_type_conv_cb;
      return cb;
    }

    // Map an HDF5 native exception type to the h5 enum.
    conv_except conv_except_of(H5T_conv_except_t except_type) {
      switch (except_type) {
        case H5T_CONV_EXCEPT_RANGE_HI: return conv_except::range_hi;
        case H5T_CONV_EXCEPT_RANGE_LOW: return conv_except::range_low;
        case H5T_CONV_EXCEPT_PRECISION: return conv_except::precision;
        case H5T_CONV_EXCEPT_TRUNCATE: return conv_except::truncate;
        case H5T_CONV_EXCEPT_PINF: return conv_except::pinf;
        case H5T_CONV_EXCEPT_NINF: return conv_except::ninf;
        case H5T_CONV_EXCEPT_NAN: return conv_except::nan;
        default: return conv_except::not_equal; // should not happen, mapped defensively
      }
    }

    // Trampoline handed to HDF5 via H5Pset_type_conv_cb. Its signature must match H5T_conv_except_func_t
    // (stable since HDF5 1.8): the src/dst data buffers are passed on to the callback, only user_data is unused.
    // It must not let C++ exceptions escape into the HDF5 C call stack, so a throwing callback is turned into
    // an abort of the conversion (which HDF5 surfaces as a failed H5Dread/H5Dwrite).
    H5T_conv_ret_t type_conv_trampoline(H5T_conv_except_t except_type, ::hid_t src_id, ::hid_t dst_id, void *src_buf,
                                        void *dst_buf, void * /*user_data*/) {
      auto const &cb = cb_ref();
      if (not cb) return H5T_CONV_UNHANDLED;
      try {
        conv_ret const r = cb(conv_except_of(except_type), object::from_borrowed(src_id), object::from_borrowed(dst_id), src_buf, dst_buf);
        return static_cast<H5T_conv_ret_t>(static_cast<int>(r));
      } catch (std::exception const &e) {
        std::cerr << "WARNING: exception thrown from the HDF5 type conversion callback was swallowed: " << e.what() << "\n";
        return H5T_CONV_ABORT;
      }
    }

    // Create the global dataset transfer property list and install the conversion callback trampoline.
    proplist make_default_transfer_plist() {
      proplist p = H5Pcreate(H5P_DATASET_XFER);
      if (not p.is_valid()) throw std::runtime_error("Error in h5: creating the global dataset transfer property list failed");
      if (H5Pset_type_conv_cb(p, &type_conv_trampoline, nullptr) < 0)
        throw std::runtime_error("Error in h5: H5Pset_type_conv_cb on the global dataset transfer property list failed");
      return p;
    }

    // Check whether converting from src to dst is a narrowing conversion.
    bool is_narrowing(datatype src, datatype dst) {
      // different classes are not comparable here (and H5Tget_precision is meaningless e.g. for compounds)
      if (H5Tget_class(src) != H5Tget_class(dst)) return false;
      // fewer bits of precision in the destination loses information
      if (H5Tget_precision(dst) < H5Tget_precision(src)) return true;
      // a change of signedness between integers can lose values even at equal precision
      if (H5Tget_class(src) == H5T_INTEGER and H5Tget_sign(src) != H5Tget_sign(dst)) return true;
      return false;
    }

  } // namespace

  void set_type_conv_cb(type_conv_cb_t cb) { cb_ref() = (cb ? std::move(cb) : type_conv_cb_t{default_type_conv_cb}); }

  type_conv_cb_t const &get_type_conv_cb() { return cb_ref(); }

  proplist &default_transfer_plist() {
    static proplist p = make_default_transfer_plist();
    return p;
  }

  namespace detail {

    void handle_type_conversion_callback(datatype src, datatype dst, std::string const &context) {
      conv_except const kind = is_narrowing(src, dst) ? conv_except::narrower : conv_except::not_equal;
      // these h5-specific (custom) cases are whole-dataset type mismatches: there is no per-element buffer, so the
      // src/dst data pointers handed to the callback are null
      if (cb_ref()(kind, src, dst, nullptr, nullptr) == conv_ret::abort) {
        throw std::runtime_error(context + ": datatype conversion aborted by callback: " + get_name_of_h5_type(src) + " -> "
                                 + get_name_of_h5_type(dst));
      }
    }

  } // namespace detail

} // namespace h5
