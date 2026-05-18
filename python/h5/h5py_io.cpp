#include <Python.h>
#include <numpy/arrayobject.h>

#include "h5py_io.hpp"
#include <h5/h5.hpp>

#include <cpp2py/cpp2py.hpp>
#include <cpp2py/converters/vector.hpp>
#include <cpp2py/converters/string.hpp>

#include <hdf5.h>
#include <hdf5_hl.h>

#include <algorithm>
#include <cstddef>
#include <complex>
#include <stdexcept>
#include <string>
#include <vector>

namespace h5 {

  // anonymous namespace for internal functions/types
  namespace {

    // Mapping between an HDF5 type and its numpy type.
    struct h5_py_type_t {
      datatype hdf5_type;
      int numpy_type;
      size_t size;
    };

    // Table of mappings between basic HDF5 types and numpy types.
    const auto h5_py_type_table = std::vector<h5_py_type_t>{
       {hdf5_type<char>(), NPY_STRING, sizeof(char)},
       {hdf5_type<signed char>(), NPY_BYTE, sizeof(signed char)},
       {hdf5_type<unsigned char>(), NPY_UBYTE, sizeof(unsigned char)},
       {hdf5_type<bool>(), NPY_BOOL, sizeof(bool)},
       {hdf5_type<short>(), NPY_SHORT, sizeof(short)},
       {hdf5_type<unsigned short>(), NPY_USHORT, sizeof(unsigned short)},
       {hdf5_type<int>(), NPY_INT, sizeof(int)},
       {hdf5_type<unsigned int>(), NPY_UINT, sizeof(unsigned int)},
       {hdf5_type<long>(), NPY_LONG, sizeof(long)},
       {hdf5_type<unsigned long>(), NPY_ULONG, sizeof(unsigned long)},
       {hdf5_type<long long>(), NPY_LONGLONG, sizeof(long long)},
       {hdf5_type<unsigned long long>(), NPY_ULONGLONG, sizeof(unsigned long long)},
       {hdf5_type<float>(), NPY_FLOAT, sizeof(float)},
       {hdf5_type<double>(), NPY_DOUBLE, sizeof(double)},
       {hdf5_type<long double>(), NPY_LONGDOUBLE, sizeof(long double)},
       {hdf5_type<std::complex<float>>(), NPY_CFLOAT, sizeof(std::complex<float>)},
       {hdf5_type<std::complex<double>>(), NPY_CDOUBLE, sizeof(std::complex<double>)},
       {hdf5_type<std::complex<long double>>(), NPY_CLONGDOUBLE, sizeof(std::complex<long double>)} //
    };

    // Given an HDF5 datatype, return the size of the corresponding C data type in bytes.
    long h5_c_size(datatype t) {
      auto pos = std::ranges::find_if(h5_py_type_table, [t](auto const &x) { return hdf5_type_equal(x.hdf5_type, t); });
      if (pos == h5_py_type_table.end()) throw std::runtime_error("HDF5/Python Error: HDF5 type not supported");
      return static_cast<long>(pos->size);
    }

    // Given an HDF5 datatype, return the corresponding numpy type.
    int h5_to_npy(datatype t, bool is_complex) {
      auto pos = std::ranges::find_if(h5_py_type_table, [t](auto const &x) { return hdf5_type_equal(x.hdf5_type, t); });
      if (pos == h5_py_type_table.end()) throw std::runtime_error("HDF5/Python Error: HDF5 type not supported");
      int res = pos->numpy_type;
      if (is_complex) {
        if (res == NPY_DOUBLE) res = NPY_CDOUBLE;
        if (res == NPY_FLOAT) res = NPY_CFLOAT;
        if (res == NPY_LONGDOUBLE) res = NPY_CLONGDOUBLE;
      }
      return res;
    }

    // Given a numpy type, return the corresponding HDF5 type.
    datatype npy_to_h5(int t) {
      auto pos = std::ranges::find_if(h5_py_type_table, [t](auto const &x) { return x.numpy_type == t; });
      if (pos == h5_py_type_table.end()) throw std::runtime_error("HDF5/Python Error: Numpy type not supported");
      return pos->hdf5_type;
    }

    // Make an h5::array_interface::array_view from a given numpy array object.
    array_interface::array_view make_av_from_npy(PyArrayObject *arr_obj) {
      // get element type and rank of numpy array
      int numpy_type = PyArray_DESCR(arr_obj)->type_num;
      int rank       = PyArray_NDIM(arr_obj);

      // get corresponding HDF5 type
      datatype dt           = npy_to_h5(numpy_type);
      const bool is_complex = (numpy_type == NPY_CDOUBLE) or (numpy_type == NPY_CLONGDOUBLE) or (numpy_type == NPY_CFLOAT);

      // initialize array view and get the shape of the array and the numpy strides
      array_interface::array_view res{dt, PyArray_DATA(arr_obj), rank, is_complex};
      std::vector<long> c_strides(rank + is_complex, 0), c_shape(rank + is_complex, 2);
      for (int i = 0; i < rank; ++i) {
        c_shape[i]        = PyArray_DIMS(arr_obj)[i];
        res.slab.count[i] = static_cast<size_t>(c_shape[i]);
        c_strides[i]      = PyArray_STRIDES(arr_obj)[i] / static_cast<long>(h5_c_size(dt));
      }

      // get the parent shape and HDF5 strides from the numpy strides
      auto [Ltot, stri] = h5::array_interface::get_parent_shape_and_h5_strides(c_strides.data(), rank + is_complex, c_shape.data());
      for (int i = 0; i < rank; ++i) {
        res.parent_shape[i] = Ltot[i];
        res.slab.stride[i]  = stri[i];
      }

      return res;
    }

    // Import numpy (only once) to use its C API.
    void import_numpy() {
      static bool init = false;
      if (!init) {
        _import_array();
        init = true;
      }
    }

    // Read any integer type from HDF5 and return a Python long.
    PyObject *h5_read_any_int(group g, std::string const &name, datatype ty) {
      if (H5Tequal(ty, H5T_NATIVE_SHORT)) {
        return PyLong_FromLong(h5_read<short>(g, name));
      } else if (H5Tequal(ty, H5T_NATIVE_INT)) {
        return PyLong_FromLong(h5_read<int>(g, name));
      } else if (H5Tequal(ty, H5T_NATIVE_LONG)) {
        return PyLong_FromLong(h5_read<long>(g, name));
      } else if (H5Tequal(ty, H5T_NATIVE_LLONG)) {
        return PyLong_FromLongLong(h5_read<long long>(g, name));
      } else if (H5Tequal(ty, H5T_NATIVE_USHORT)) {
        return PyLong_FromUnsignedLong(h5_read<unsigned short>(g, name));
      } else if (H5Tequal(ty, H5T_NATIVE_UINT)) {
        return PyLong_FromUnsignedLong(h5_read<unsigned int>(g, name));
      } else if (H5Tequal(ty, H5T_NATIVE_ULONG)) {
        return PyLong_FromUnsignedLong(h5_read<unsigned long>(g, name));
      } else if (H5Tequal(ty, H5T_NATIVE_ULLONG)) {
        return PyLong_FromUnsignedLongLong(h5_read<unsigned long long>(g, name));
      } else {
        PyErr_SetString(PyExc_RuntimeError, "h5::h5_read_bare: Integer type can not be read from HDF5");
        return nullptr;
      }
    }

  } // namespace

  void h5_write_bare(group g, std::string const &name, PyObject *ob) {
    import_numpy();

    if (PyArray_Check(ob)) {
      auto *arr_obj = (PyArrayObject *)ob; // NOLINT
      write(g, name, make_av_from_npy(arr_obj), true);
    } else if (PyArray_CheckScalar(ob)) {
      // treat numpy scalars as 0-dimensional ndarrays
      cpp2py::pyref obsc = PyArray_FromScalar(ob, nullptr);
      h5_write_bare(g, name, obsc);
    } else if (PyFloat_Check(ob)) {
      h5_write(g, name, PyFloat_AsDouble(ob));
    } else if (PyBool_Check(ob)) {
      h5_write(g, name, static_cast<bool>(PyLong_AsLong(ob)));
    } else if (PyLong_Check(ob)) {
      h5_write(g, name, static_cast<long>(PyLong_AsLong(ob)));
    } else if (PyUnicode_Check(ob)) {
      h5_write(g, name, static_cast<const char *>(PyUnicode_AsUTF8(ob)));
    } else if (PyComplex_Check(ob)) {
      h5_write(g, name, std::complex<double>{PyComplex_RealAsDouble(ob), PyComplex_ImagAsDouble(ob)});
    } else {
      PyErr_SetString(PyExc_RuntimeError, "h5::h5_write_bare: Python object can not be written to HDF5");
      return;
    }
  }

  PyObject *h5_read_bare(group g, std::string const &name) {
    import_numpy();

    auto ds_info = array_interface::get_dataset_info(g, name);

    // rank 0 - scalar case
    if (ds_info.rank() == 0) {
      // float
      if (H5Tget_class(ds_info.ty) == H5T_FLOAT) {
        double x{};
        h5_read(g, name, x);
        return PyFloat_FromDouble(x);
      }

      // integer
      if (H5Tget_class(ds_info.ty) == H5T_INTEGER) { return h5_read_any_int(g, name, ds_info.ty); }

      // bool
      if (H5Tequal(ds_info.ty, h5::hdf5_type<bool>())) {
        bool x{};
        h5_read(g, name, x);
        return PyBool_FromLong(long(x));
      }

      // string
      if (H5Tget_class(ds_info.ty) == H5T_STRING) {
        std::string x;
        h5_read(g, name, x);
        return PyUnicode_FromString(x.c_str());
      }

      // complex
      if (H5Tequal(ds_info.ty, hdf5_type<dcplx_t>())) {
        dcplx_t x{};
        h5_read(g, name, x);
        return PyComplex_FromDoubles(x.r, x.i);
      }

      // otherwise throw and error and return nullptr
      PyErr_SetString(PyExc_RuntimeError, "h5::h5_read_bare: Scalar type can not be read from HDF5");
      return nullptr;
    }

    // rank 1 - complex scalar case
    if ((ds_info.rank() == 1) and ds_info.has_complex_attribute) {
      std::complex<double> z{};
      h5_read(g, name, z);
      return PyComplex_FromDoubles(z.real(), z.imag());
    }

    // rank 1 or 2 - string array case
    if (H5Tget_class(ds_info.ty) == H5T_STRING) {
      if (ds_info.rank() == 1) {
        auto x = h5_read<std::vector<std::string>>(g, name);
        return cpp2py::convert_to_python(x);
      }

      if (ds_info.rank() == 2) {
        auto x = h5_read<std::vector<std::vector<std::string>>>(g, name);
        return cpp2py::convert_to_python(x);
      }

      PyErr_SetString(PyExc_RuntimeError, "h5::h5_read_bare: String dataset with rank > 2 is not allowed");
      return nullptr;
    }

    // rank > 0 - general array case
    auto shape      = std::vector<npy_intp>(ds_info.lengths.begin(), ds_info.lengths.end());
    auto numpy_type = h5_to_npy(ds_info.ty, ds_info.has_complex_attribute);

    // get rid of complex h5 dimension if necessary
    if (ds_info.has_complex_attribute) shape.pop_back();

    // create numpy array and read into it
    PyObject *ob = PyArray_SimpleNewFromDescr(int(shape.size()), &shape[0], PyArray_DescrFromType(numpy_type));
    if (PyErr_Occurred()) return nullptr;
    read(g, name, make_av_from_npy((PyArrayObject *)ob)); // NOLINT
    return ob;
  }

} // namespace h5
