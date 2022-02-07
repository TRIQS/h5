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

#ifndef LIBH5_OBJECT_HPP
#define LIBH5_OBJECT_HPP

#include <complex>
#include <vector>
#include <sstream>
#include "./macros.hpp"

namespace h5 {

  // We copy this from hdf5.h, and static_assert its validity in the cpp
  // in order to completely isolate our header from the hdf5 headers
  // Hence complex installation paths to hdf5 are only needed in the cpp file,
  // not by the users of the library.
  using hid_t   = int64_t;
  using hsize_t = unsigned long long;
  using v_t     = std::vector<hsize_t>;

  // A complex compound type consisting of two doubles
  // This type is stored and loaded as an hdf5 compound datatype
  struct dcplx_t {
    double r, i;
  };

  // impl trait to detect complex numbers
  template <typename T>
  struct _is_complex : std::false_type {};

  template <typename T>
  struct _is_complex<std::complex<T>> : std::true_type {};

  template <typename T>
  constexpr bool is_complex_v = _is_complex<T>::value;

  // impl
  template <typename... T>
  std::runtime_error make_runtime_error(T const &...x) {
    std::stringstream fs;
    (fs << ... << x);
    return std::runtime_error{fs.str()};
  }

  /*
   * A handle to the a general HDF5 object
   *
   * HDF5 uses a reference counting system, similar to python.
   * h5::object handles the proper reference couting (similar to pybind11::object e.g.)
   * using a RAII pattern (hence exception safety).
   */
  class object {

    protected:
    hid_t id = 0; //NOLINT Ok, I want a protected variable ...

    public:
    /// make an h5::object from a simple borrowed ref (simply inc. the ref).
    static object from_borrowed(hid_t id);

    /// Constructor from an owned id (or 0). It steals (take ownership) of the reference.
    object(hid_t id = 0) : id(id) {}

    /// A new ref. No deep copy.
    object(object const &x);

    /// Steals the reference
    object(object &&x) noexcept : id(x.id) { x.id = 0; }

    /// Copy the reference and incref
    object &operator=(object const &x) {
      operator=(object(x));
      return *this;
    }

    /// Steals the ref.
    object &operator=(object &&x) noexcept;

    ///
    ~object() { close(); }

    /// Release the HDF5 handle and reset the object to default state (id =0).
    void close();

    /// cast operator to use it in the C function as its id
    operator hid_t() const { return id; }

    /// Get the current number of references
    [[nodiscard]] int get_ref_count() const;

    /// Ensure the id is valid (by H5Iis_valid).
    [[nodiscard]] bool is_valid() const;
  };

  //-----------------------------

  // simple but useful aliases. It does NOT check the h5 type of the object.
  // FIXME : derive and make a check ??
  using dataset   = object;
  using datatype  = object;
  using dataspace = object;
  using proplist  = object;
  using attribute = object;

  // -----------------------  hdf5_type  ---------------------------
  // Correspondance T -> hdf5 type
  namespace detail {
    template <typename T>
    hid_t hid_t_of();
  }

  template <typename T>
  datatype hdf5_type() {
    return object::from_borrowed(detail::hid_t_of<T>());
  }

  // ------------------------------

  // A function to get the name of a datatype in clear (for error messages)
  std::string get_name_of_h5_type(datatype ty);

  // Get hdf5 type of a dataset
  object get_hdf5_type(dataset);

  // Check equality of datatypes
  bool hdf5_type_equal(datatype, datatype);

} // namespace h5

#endif // LIBH5_OBJECT_HPP
