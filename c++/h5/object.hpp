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

/**
 * @file
 * @brief Provides a generic handle for HDF5 objects.
 */

#ifndef LIBH5_OBJECT_HPP
#define LIBH5_OBJECT_HPP

#include "./utils.hpp"

#include <string>

namespace h5 {

  /**
   * @addtogroup data_model
   * @{
   */

  /**
   * @brief A generic handle for HDF5 objects.
   *
   * @details It simply stores the h5::hid_t of the corresponding HDF5 object.
   *
   * More specific HDF5 objects, like h5::file or h5::group, inherit from this class. Since it lacks a virtual
   * destructor, the derived classes should not be deleted through a pointer to this class. It is recommended
   * to use the derived classes whenever possible.
   *
   * HDF5's reference counting system is similar to Python's. This class handles the proper reference counting
   * using a RAII pattern (hence exception safe). Depending on how the object is constructed, it either
   * increases the reference count associated with the HDF5 object or steals it.
   */
  class object {
    protected:
    // Wrapped HDF5 ID.
    hid_t id = 0; // NOLINT (protected member is wanted here)

    public:
    /**
     * @brief Create an h5::object for a given HDF5 ID and increase its reference count.
     *
     * @param id HDF5 ID.
     * @return h5::object.
     */
    [[nodiscard]] static object from_borrowed(hid_t id);

    /**
     * @brief Construct a new h5::object for a given HDF5 ID by taking ownership, i.e. without increasing the
     * reference count.
     *
     * @details If no ID is given, it is set to zero (default).
     *
     * @param id HDF5 ID.
     */
    object(hid_t id = 0) : id(id) {}

    /**
     * @brief Copy constructor copies the underlying HDF5 ID and increases its reference count.
     * @param x Object to copy.
     */
    object(object const &x);

    /**
     * @brief Move constructor steals the underlying HDF5 ID without increasing its reference count.
     * @param x Object to move.
     */
    object(object &&x) noexcept : id(x.id) { x.id = 0; }

    /**
     * @brief Copy assignment operator copies the underlying HDF5 ID and increases its reference count.
     * @param x Object to copy.
     */
    object &operator=(object const &x) {
      operator=(object(x));
      return *this;
    }

    /**
     * @brief Move assignment operator steals the underlying HDF5 ID without increasing its reference count.
     * @param x Object to move.
     */
    object &operator=(object &&x) noexcept;

    /// Destructor decreases the reference count and sets the object's ID to zero.
    ~object() { close(); }

    /// Release the HDF5 handle by decreasing the reference count and by setting the object's ID to zero.
    void close();

    /// User-defined conversion to h5::hid_t.
    operator hid_t() const { return id; }

    /// Get the current reference count.
    [[nodiscard]] int get_ref_count() const;

    /// Ensure that the wrapped HDF5 ID is valid (by calling `H5Iis_valid`).
    [[nodiscard]] bool is_valid() const;
  };

  // simple but useful aliases. It does NOT check the h5 type of the object.
  // FIXME : derive and make a check ??

  /// Type alias for an HDF5 dataset.
  using dataset = object;

  /// Type alias for an HDF5 datatype.
  using datatype = object;

  /// Type alias for an HDF5 dataspace.
  using dataspace = object;

  /// Type alias for an HDF5 property list.
  using proplist = object;

  /// Type alias for an HDF5 attribute.
  using attribute = object;

  /** @} */

  /**
   * @addtogroup h5_types
   * @{
   */

  namespace detail {

    // Map a C++ type to an HDF5 type (specializations are in object.cpp).
    template <typename T>
    hid_t hid_t_of();

  } // namespace detail

  /**
   * @brief Map a given C++ type to an HDF5 datatype.
   *
   * @tparam T C++ type.
   * @return h5::datatype object corresponding to the given C++ type.
   */
  template <typename T>
  [[nodiscard]] datatype hdf5_type() {
    return object::from_borrowed(detail::hid_t_of<T>());
  }

  /**
   * @brief Get the name of an h5::datatype (for error messages).
   *
   * @details Throws an exception if the datatype is not supported.
   *
   * @param dt h5::datatype.
   * @return String representation of the datatype.
   */
  [[nodiscard]] std::string get_name_of_h5_type(datatype dt);

  /**
   * @brief Get the HDF5 type stored in a given h5::dataset.
   *
   * @param ds h5::dataset.
   * @return h5::datatype of the given h5::dataset.
   */
  [[nodiscard]] datatype get_hdf5_type(dataset ds);

  /**
   * @brief Check if two HDF5 datatypes are equal.
   *
   * @details For string types, this function only checks if they are both of the class `H5T_STRING`.
   * It does not take into account the size, character set, etc.
   *
   * Otherwise, it simply uses `H5Tequal`. Throws an exception if the HDF5 call fails.
   *
   * @param dt1 h5::datatype #1.
   * @param dt2 h5::datatype #2.
   * @return True, if the two datatypes are equal.
   */
  [[nodiscard]] bool hdf5_type_equal(datatype dt1, datatype dt2);

  /** @} */

} // namespace h5

#endif // LIBH5_OBJECT_HPP
