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
 * @brief Provides a library-global dataset transfer property list and a
 * user-overridable datatype conversion callback.
 */

#ifndef LIBH5_TRANSFER_HPP
#define LIBH5_TRANSFER_HPP

#include "./object.hpp"

#include <functional>

namespace h5 {

  /**
   * @addtogroup data_model
   * @{
   */

  /**
   * @brief Possible return values of an h5::type_conv_cb_t.
   *
   * @details Mirrors HDF5's `H5T_conv_ret_t`. It tells HDF5 (and, for the h5-specific exceptions,
   * the h5 library) how to proceed after the callback has run.
   */
  enum class conv_ret {
    abort     = -1, ///< Abort the conversion (h5 turns this into a thrown exception).
    unhandled = 0,  ///< The callback did not handle the exception; fall back to HDF5's default behavior.
    handled   = 1   ///< The callback handled the exception.
  };

  /**
   * @brief Datatype conversion situations that can trigger the conversion callback.
   *
   * @details The first block mirrors HDF5's `H5T_conv_except_t`. These are genuine per-element conversion
   * exceptions raised by HDF5 during `H5Dread`/`H5Dwrite` and delivered through the callback installed on the
   * global transfer property list via `H5Pset_type_conv_cb`.
   *
   * The last two values (#not_equal and #narrower) are h5 additions. HDF5 never raises them on its own (it only
   * reports per-value exceptions, not the mere fact that two datatypes differ), so the h5 library detects these
   * cases and invokes the callback manually at the read/write call sites.
   */
  enum class conv_except {
    range_hi,  ///< Source value is above the range of the destination type.
    range_low, ///< Source value is below the range of the destination type.
    precision, ///< Precision was lost in the conversion.
    truncate,  ///< A floating point value was truncated during the conversion.
    pinf,      ///< Source value is positive infinity.
    ninf,      ///< Source value is negative infinity.
    nan,       ///< Source value is not a number.
    not_equal, ///< (h5) The source and destination datatypes are not exactly equal.
    narrower   ///< (h5) The destination datatype is narrower than the source datatype.
  };

  /**
   * @brief Type of the user-overridable datatype conversion callback.
   *
   * @details `src` is the source datatype, `dst` the destination datatype: on read, the file type is the
   * source and the in-memory type the destination; on write it is the other way around.
   *
   * `src_buf` and `dst_buf` are the void pointers to the source and destination data being converted (paired
   * with `src`/`dst`). For HDF5's native per-element exceptions they point to the offending source element and
   * its destination slot, so a callback may inspect or patch the value in place. For the h5-specific
   * #conv_except::not_equal and #conv_except::narrower cases (raised manually by the h5 library at the
   * read/write call sites, see detail::handle_type_conversion_callback) **both pointers are `nullptr`**: those
   * are whole-dataset datatype mismatches with no single offending element to point at.
   */
  using type_conv_cb_t = std::function<conv_ret(conv_except except, datatype src, datatype dst, void *src_buf, void *dst_buf)>;

  /**
   * @brief Set the library-global datatype conversion callback.
   *
   * @details The callback is consulted for HDF5's native per-element conversion exceptions (via the global
   * transfer property list, see h5::default_transfer_plist) as well as for the h5-specific #conv_except::not_equal
   * and #conv_except::narrower situations detected during reading/writing.
   *
   * The callback is a single process-global: setting it replaces the previous one everywhere, there is no
   * per-file or per-call scoping, and it is not synchronized against concurrent I/O. Note that HDF5 delivers its
   * native exceptions once per offending element, so a callback that logs may be invoked many times for a single
   * lossy read.
   *
   * Passing an empty std::function restores the built-in default, which prints a warning to `std::cerr` for the
   * #conv_except::not_equal and #conv_except::narrower cases and stays silent (returns #conv_ret::unhandled) for
   * HDF5's native exceptions.
   *
   * @param cb Callback to install, or `{}` to restore the default.
   */
  void set_type_conv_cb(type_conv_cb_t cb);

  /**
   * @brief Get the currently installed datatype conversion callback.
   * @return The current h5::type_conv_cb_t.
   */
  [[nodiscard]] type_conv_cb_t const &get_type_conv_cb();

  /**
   * @brief Get the library-global dataset transfer property list.
   *
   * @details Used as the transfer property list argument in all `H5Dread`/`H5Dwrite` calls. It is created lazily
   * (as an `H5P_DATASET_XFER` property list) on first use and has the conversion callback trampoline installed via
   * `H5Pset_type_conv_cb`. Advanced users may set additional transfer properties on it.
   *
   * @return Reference to the global h5::proplist.
   */
  [[nodiscard]] proplist &default_transfer_plist();

  namespace detail {

    /**
     * @brief Report a datatype mismatch through the conversion callback, throwing if the callback aborts.
     *
     * @details Classifies the mismatch as #conv_except::narrower (if the destination is narrower than the source)
     * or #conv_except::not_equal otherwise, invokes the global callback, and throws a std::runtime_error if the
     * callback returns #conv_ret::abort.
     *
     * @param src Source datatype.
     * @param dst Destination datatype.
     * @param context Prefix for the exception message (e.g. the calling function).
     */
    void handle_type_conversion_callback(datatype src, datatype dst, std::string const &context);

  } // namespace detail

  /** @} */

} // namespace h5

#endif // LIBH5_TRANSFER_HPP
