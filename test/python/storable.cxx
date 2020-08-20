#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "./storable.hpp"

namespace py = pybind11;

template <typename T>
std::function<T(h5::group, std::string const &)> make_py_h5_reader() {
  auto reader = [](h5::group g, std::string const &name) -> T { return h5::h5_read<T>(g, name); };
  return {reader};
}

PYBIND11_MODULE(storable, MODULE) {

  py::class_<storable> Storable(MODULE, "Storable");

  Storable.def(py::init<>(), "Constructor")
     .def_readwrite("vec", &storable::vec, "")
     .def_readwrite("s", &storable::s, "")
     .def("hdf5_format", &storable::hdf5_format, "");

  Storable.def(
     "__write_hdf5__", [](storable const &x, h5::group g, std::string const &key) { h5_write(g, key, x); }, "h5 writing");

  py::object h5_formats       = py::module::import("h5.formats");
  py::function register_class = py::object{h5_formats.attr("register_class")};

#if 0
  std::function<storable(h5::group, std::string const &)> f = ;
  py::object read_fun = py::cast(make_py_h5_reader<storable>());
  py::object format = py::cast(h5::get_hdf5_format<storable>());
  py::object mytype = MODULE.attr("Storable");
  register_class(mytype, py::none(), read_fun, format);
#else

  register_class(MODULE.attr("Storable"), py::none(), make_py_h5_reader<storable>(), h5::get_hdf5_format<storable>());

#endif
}
