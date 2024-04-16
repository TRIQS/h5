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

#include "./complex.hpp"
#include "./macros.hpp"
#include "./object.hpp"

#include <H5Ipublic.h>
#include <H5Fpublic.h>
#include <H5Gpublic.h>
#include <H5Ppublic.h>

#include <algorithm>
#include <complex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

// make sure that hid_t and hsize_t have a compatible type in the HDF5 library
static_assert(std::is_same_v<hid_t, int64_t> or std::is_same_v<hid_t, int>, "h5::hid_t type is not compatible with HDF5");
static_assert(sizeof(::hsize_t) == sizeof(h5::hsize_t), "h5::hsize_t type is not compatible with HDF5");

namespace h5 {

  // anonymous namespace for internal functions
  namespace {

    // decrease reference count if HDF5 ID is valid
    inline void xdecref(hid_t id) {
      if (H5Iis_valid(id)) H5Idec_ref(id);
    }

    // increase reference count if HDF5 ID is valid
    inline void xincref(hid_t id) {
      if (H5Iis_valid(id)) H5Iinc_ref(id);
    }

    // stores an HDF5 datatype and its name
    struct h5_name_t {
      datatype hdf5_type;
      std::string name;
    };

    // table of HDF5 datatypes and their names
    std::vector<h5_name_t> h5_name_table; // NOLINT (global variable is in an unnamed namespace)

    // initialize the table of HDF5 datatypes and their names
    void init_h5_name_table() {
      h5_name_table = std::vector<h5_name_t>{
         {hdf5_type<char>(), H5_AS_STRING(char)},
         {hdf5_type<signed char>(), H5_AS_STRING(signed char)},
         {hdf5_type<unsigned char>(), H5_AS_STRING(unsigned char)},
         {hdf5_type<bool>(), H5_AS_STRING(bool)},
         {hdf5_type<short>(), H5_AS_STRING(short)},
         {hdf5_type<unsigned short>(), H5_AS_STRING(unsigned short)},
         {hdf5_type<int>(), H5_AS_STRING(int)},
         {hdf5_type<unsigned int>(), H5_AS_STRING(unsigned int)},
         {hdf5_type<long>(), H5_AS_STRING(long)},
         {hdf5_type<unsigned long>(), H5_AS_STRING(unsigned long)},
         {hdf5_type<long long>(), H5_AS_STRING(long long)},
         {hdf5_type<unsigned long long>(), H5_AS_STRING(unsigned long long)},
         {hdf5_type<float>(), H5_AS_STRING(float)},
         {hdf5_type<double>(), H5_AS_STRING(double)},
         {hdf5_type<long double>(), H5_AS_STRING(long double)},
         {hdf5_type<std::complex<float>>(), H5_AS_STRING(std::complex<float>)},
         {hdf5_type<std::complex<double>>(), H5_AS_STRING(std::complex<double>)},
         {hdf5_type<std::complex<long double>>(), H5_AS_STRING(std::complex<long double>)},
         {hdf5_type<std::string>(), H5_AS_STRING(std::string)},
         {hdf5_type<dcplx_t>(), "Complex Compound Datatype"} //
      };
    }

  } // namespace

  object object::from_borrowed(hid_t id) {
    xincref(id);
    return {id};
  }

  object::object(object const &x) : id(x.id) { xincref(id); }

  object &object::operator=(object &&x) noexcept {
    xdecref(id);
    id   = x.id;
    x.id = 0;
    return *this;
  }

  void object::close() {
    xdecref(id);
    id = 0;
  }

  int object::get_ref_count() const { return H5Iget_ref(id); }

  bool object::is_valid() const { return H5Iis_valid(id) == 1; }

  namespace detail {

    // clang-format off
    // map basic C++ types to HDF5 datatypes
    template <> hid_t hid_t_of<char>          (){return  H5T_NATIVE_CHAR;}
    template <> hid_t hid_t_of<signed char>   (){return  H5T_NATIVE_SCHAR;}
    template <> hid_t hid_t_of<unsigned char> (){return  H5T_NATIVE_UCHAR;}

    template <> hid_t hid_t_of<short>     (){return  H5T_NATIVE_SHORT;}
    template <> hid_t hid_t_of<int>       (){return  H5T_NATIVE_INT;}
    template <> hid_t hid_t_of<long>      (){return  H5T_NATIVE_LONG;}
    template <> hid_t hid_t_of<long long> (){return  H5T_NATIVE_LLONG;}

    template <> hid_t hid_t_of<unsigned short>     (){return  H5T_NATIVE_USHORT;}
    template <> hid_t hid_t_of<unsigned int>       (){return  H5T_NATIVE_UINT;}
    template <> hid_t hid_t_of<unsigned long>      (){return  H5T_NATIVE_ULONG;}
    template <> hid_t hid_t_of<unsigned long long> (){return  H5T_NATIVE_ULLONG;}

    template <> hid_t hid_t_of<float>       (){return  H5T_NATIVE_FLOAT;}
    template <> hid_t hid_t_of<double>      (){return  H5T_NATIVE_DOUBLE;}
    template <> hid_t hid_t_of<long double> (){return  H5T_NATIVE_LDOUBLE;}

    template <> hid_t hid_t_of<std::complex<float>>       (){return  H5T_NATIVE_FLOAT;}
    template <> hid_t hid_t_of<std::complex<double>>      (){return  H5T_NATIVE_DOUBLE;}
    template <> hid_t hid_t_of<std::complex<long double>> (){return  H5T_NATIVE_LDOUBLE;}

    // custom HDF5 datatype for strings
    hid_t const str_dt = [](){
      hid_t dt = H5Tcopy(H5T_C_S1);
      H5Tset_size(dt, H5T_VARIABLE);
      H5Tset_cset(dt, H5T_CSET_UTF8);
      H5Tlock(dt);
      return dt;
    }();

    // map different string types to the custom HDF5 string datatype from above
    template <> hid_t hid_t_of<std::string>  (){return  detail::str_dt;}
    template <> hid_t hid_t_of<char *>       (){return  detail::str_dt;}
    template <> hid_t hid_t_of<const char *> (){return  detail::str_dt;}

    // custom HDF5 datatype for a complex number
    hid_t const cplx_cmpd_dt = [](){
      hid_t dt = H5Tcreate(H5T_COMPOUND, 16);
      H5Tinsert(dt, "r", 0, H5T_NATIVE_DOUBLE);
      H5Tinsert(dt, "i", 8, H5T_NATIVE_DOUBLE);
      H5Tlock(dt);
      return dt;
    }();

    // map the custom complex type to the custom HDF5 datatype from above
    template <> hid_t hid_t_of<dcplx_t>  (){return  detail::cplx_cmpd_dt;}
    // clang-format on

    // custom HDF5 datatype for bool (use a lambda to initialize it?)
    template <>
    hid_t hid_t_of<bool>() {
      hid_t bool_enum_h5type = H5Tenum_create(H5T_NATIVE_CHAR);
      char val               = 0;
      H5Tenum_insert(bool_enum_h5type, "FALSE", (val = 0, &val));
      H5Tenum_insert(bool_enum_h5type, "TRUE", (val = 1, &val));
      return bool_enum_h5type;
    }

  } // namespace detail

  std::string get_name_of_h5_type(datatype dt) {
    // initialize name table if it has not been done yet
    if (h5_name_table.empty()) init_h5_name_table();

    // find name in table
    auto _end = h5_name_table.end();
    auto pos  = std::find_if(h5_name_table.begin(), _end, [dt](auto const &x) { return hdf5_type_equal(dt, x.hdf5_type); });

    // return name if found, otherwise throw an exception
    if (pos == _end) throw std::logic_error("Error in h5::get_name_of_h5_type: datatype not supported");
    return pos->name;
  }

  datatype get_hdf5_type(dataset ds) { return H5Dget_type(ds); }

  bool hdf5_type_equal(datatype dt1, datatype dt2) {
    // for strings check only if they are both of the class H5T_STRING
    if (H5Tget_class(dt1) == H5T_STRING) { return H5Tget_class(dt2) == H5T_STRING; }

    // for other types use H5Tequal
    auto res = H5Tequal(dt1, dt2);
    if (res < 0) { throw std::runtime_error("Error in h5::hdf5_type_equal: H5Tequal call failed"); }
    return res > 0;
  }

} // namespace h5
