#include <Python.h>

#include <h5/file.hpp>
#include <h5/group.hpp>
#include <h5/scalar.hpp>
#include <h5/stl/string.hpp>
#include <h5/array_interface.hpp>

#include <hdf5.h>
#include <hdf5_hl.h>

using namespace std::string_literals;
#define CHECK_OR_THROW(Cond, Mess)                                                                                                                   \
  if (!(Cond)) throw std::runtime_error("Error in h5 interface : "s + Mess);

#define AS_STRING(...) AS_STRING2(__VA_ARGS__)
#define AS_STRING2(...) #__VA_ARGS__

#define PRINT(X) std::cerr << AS_STRING(X) << " = " << X << "      at " << __FILE__ << ":" << __LINE__ << '\n'

namespace h5 {

  // -------------------------
  // implementation of the py part of memoryfile
  // -------------------------

  memory_file::memory_file(PyObject *py_byte_array)
     : memory_file{(unsigned char *)PyByteArray_AsString(py_byte_array), PyByteArray_Size(py_byte_array)} {
    CHECK_OR_THROW(PyByteArray_CheckExact(py_byte_array), "h5::memory_file : the PyObject passed is not a PyByteArray !");
  }

  // -------------------------

  PyObject *memory_file::as_py_byte_array() const {
    size_t image_len        = get_size();
    PyObject *py_byte_array = PyByteArray_FromStringAndSize(" ", Py_ssize_t(1));
    //PyObject *py_byte_array  = PyByteArray_FromStringAndSize(" ", Py_ssize_t(image_len));
    int err = PyByteArray_Resize(py_byte_array, Py_ssize_t(image_len));
    CHECK_OR_THROW(not err, "Can not resize the PyByteArray ?!");
    CHECK_OR_THROW((PyByteArray_Size(py_byte_array) == image_len), "Incorrect size of PyByteArray");

    //hssize_t free_space = H5Fget_freespace(this->id);
    //PRINT(free_space);
    //PRINT(PyByteArray_Size(py_byte_array));
    write_to_buffer((unsigned char *)PyByteArray_AsString(py_byte_array), image_len);
    return py_byte_array;
  }

  // -----------------------------

} // namespace h5
