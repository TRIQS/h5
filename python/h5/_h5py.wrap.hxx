#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION__h5py_GUARDS
#define C2PY_HXX_DECLARATION__h5py_GUARDS
template <>
constexpr bool c2py::is_wrapped<h5::file> = true;
template <>
inline constexpr auto c2py::tp_name<h5::file> = "h5._h5py.File";
template <>
constexpr bool c2py::is_wrapped<h5::group> = true;
template <>
inline constexpr auto c2py::tp_name<h5::group> = "h5._h5py.Group";
#endif