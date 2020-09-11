#ifndef LIBH5_H5PY_IO_HPP
#define LIBH5_H5PY_IO_HPP

#include <h5/h5.hpp>
#include <h5/array_interface.hpp>
#include <Python.h>

namespace h5 {

  void h5_write_bare(group g, std::string const &name, PyObject *ob);

  // -----------------------------

  PyObject *h5_read_bare(group g, std::string const &name);


} // namespace h5

#endif // LIBH5_H5PY_IO_HPP
