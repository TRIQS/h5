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

#include <type_traits>

#include <H5Ipublic.h>
#include <H5Fpublic.h>
#include <H5Gpublic.h>
#include <H5Ppublic.h>

// FIXME
//static_assert(std::is_same_v<hid_t, int64_t>, "Configuration error in HDF5. Check version.");

#include "./object.hpp"
#include <vector>
#include <algorithm>
#include <string>

namespace h5 {

  namespace detail {

    //static_assert(std::is_same<::hid_t, hid_t>::value, "Internal error");
    static_assert(std::is_same<::hsize_t, hsize_t>::value, "Internal error");

    // specializations for all basic types
    // clang-format off
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

    hid_t const str_dt = [](){
      hid_t dt = H5Tcopy(H5T_C_S1);
      H5Tset_size(dt, H5T_VARIABLE);
      H5Tset_cset(dt, H5T_CSET_UTF8);
      H5Tlock(dt);
      return dt;
    }();
  
    template <> hid_t hid_t_of<std::string>  (){return  detail::str_dt;}
    template <> hid_t hid_t_of<char *>       (){return  detail::str_dt;}
    template <> hid_t hid_t_of<const char *> (){return  detail::str_dt;}
    
    hid_t const cplx_cmpd_dt = [](){
      hid_t dt = H5Tcreate(H5T_COMPOUND, 16);
      H5Tinsert(dt, "r", 0, H5T_NATIVE_DOUBLE);
      H5Tinsert(dt, "i", 8, H5T_NATIVE_DOUBLE);
      H5Tlock(dt);
      return dt;
    }();
    template <> hid_t hid_t_of<dcplx_t>  (){return  detail::cplx_cmpd_dt;}
    // clang-format on

    // bool. Use a lambda to initialize it.
    template <>
    hid_t hid_t_of<bool>() {
      hid_t bool_enum_h5type = H5Tenum_create(H5T_NATIVE_CHAR);
      char val = 0;
      H5Tenum_insert(bool_enum_h5type, "FALSE", (val = 0, &val));
      H5Tenum_insert(bool_enum_h5type, "TRUE", (val = 1, &val));
      return bool_enum_h5type;
    }
  } // namespace detail

  // -----------------------  name  ---------------------------

  struct h5_name_t {
    datatype hdf5_type; // type in hdf5
    std::string name;   // name of the type
  };

  //---------

  std::vector<h5_name_t> h5_name_table;

  //----------

  static void init_h5_name_table() {
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

  //--------

  object get_hdf5_type(dataset ds) { return H5Dget_type(ds); }

  bool hdf5_type_equal(datatype dt1, datatype dt2) {
    // For string do not compare size, cset..
    if (H5Tget_class(dt1) == H5T_STRING) { return H5Tget_class(dt2) == H5T_STRING; }
    auto res = H5Tequal(dt1, dt2);
    if (res < 0) { throw std::runtime_error("Failure it hdf5 type comparison"); }
    return res > 0;
  }

  std::string get_name_of_h5_type(datatype t) {

    if (h5_name_table.empty()) init_h5_name_table();
    auto _end = h5_name_table.end();
    auto pos  = std::find_if(h5_name_table.begin(), _end, [t](auto const &x) { return hdf5_type_equal(t, x.hdf5_type); });
    if (pos == _end) throw std::logic_error("HDF5/Python : impossible error");
    return pos->name;
  }

  // -----------------------   Reference counting ---------------------------

  // xdecref, xincref manipulate the the ref, but ignore invalid (incl. 0) id.
  //  like XINC_REF and XDEC_REF in python
  inline void xdecref(hid_t id) {
    if (H5Iis_valid(id)) H5Idec_ref(id);
  }

  inline void xincref(hid_t id) {
    if (H5Iis_valid(id)) H5Iinc_ref(id);
  }

  // -----------------------  object  ---------------------------

  object::object(object const &x) : id(x.id) { xincref(id); } // a new copy, a new ref.

  // make an object when the id is now owned (simply inc. the ref).
  object object::from_borrowed(hid_t id) {
    xincref(id);
    return {id};
  }

  object &object::operator=(object &&x) noexcept { //steals the ref, after properly decref its own.
    xdecref(id);
    id   = x.id;
    x.id = 0;
    return *this;
  }

  void object::close() {
    xdecref(id);
    id = 0;
  } // e.g. to close a file explicitely.

  int object::get_ref_count() const { return H5Iget_ref(id); }

  bool object::is_valid() const { return H5Iis_valid(id) == 1; }

} // namespace h5
