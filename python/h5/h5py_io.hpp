/**
 * @file
 * @brief Provides helper functions to read/write Python objects to/from HDF5 files.
 */

#ifndef LIBH5_H5PY_IO_HPP
#define LIBH5_H5PY_IO_HPP

#include <c2py/c2py.hpp>
#include <Python.h>

#include <h5/group.hpp>
#include <h5/array_interface.hpp>

namespace h5 {

  /**
   * @addtogroup python-utils
   * @{
   */

  /**
   * @brief Write a Python object to an HDF5 group.
   *
   * @details Only scalars, strings and numpy arrays are supported. They are first converted to a corresponding C++ type
   * and then written to HDF5 using the functionality provided by **h5**:
   * - numpy ndarray \f$ \rightarrow \f$ h5::array_interface::array_view
   * - numpy scalar \f$ \rightarrow \f$ 0-dimensional numpy array \f$ \rightarrow \f$ h5::array_interface::array_view
   * - Python float \f$ \rightarrow \f$ `double`
   * - Python bool \f$ \rightarrow \f$ `bool`
   * - Python int \f$ \rightarrow \f$ `long`
   * - Python str \f$ \rightarrow \f$ `const char *`
   * - Python complex \f$ \rightarrow \f$ `std::complex<double>`
   *
   * @param g h5::group in which the dataset is created.
   * @param name Name of the dataset to which the object is written.
   * @param ob Python object to be written.
   */
  void h5_write_bare(group g, std::string const &name, PyObject *ob);

  /**
   * @brief Read a dataset from an HDF5 group and return it as a Python object.
   *
   * @details The Python type is determined from the dataset's HDF5 type and its rank:
   * - Rank 0:
   *   - `H5T_FLOAT` \f$ \rightarrow \f$ Python float
   *   - `H5T_INTEGER` \f$ \rightarrow \f$ Python int
   *   - `H5T_STRING` \f$ \rightarrow \f$ Python str
   * - Rank 1 and h5::array_interface::dataset_info has complex attribute \f$ \rightarrow \f$ Python complex
   * - Rank 0, 1 or 2 and `H5T_STRING` \f$ \rightarrow \f$ Python str or list of (list of) str
   * - Otherwise \f$ \rightarrow \f$ numpy ndarray
   *
   * @param g h5::group containing the dataset.
   * @param name Name of the dataset from which the object is read.
   * @return `c2py::pyref` wrapping the Python object (new reference).
   */
  c2py::pyref h5_read_bare(group g, std::string const &name);

  /** @} */

} // namespace h5

#endif // LIBH5_H5PY_IO_HPP
