#include <c2py/c2py.hpp>
#include "./h5py_io.hpp"

namespace c2py_module {

  auto match_names = "h5::(group|file)";
  auto reject_names = "create_dataset|open_dataset|from_borrowed";
  auto package_name = "_h5py";

  namespace add {
    auto h5_write = c2py::dispatch<h5::h5_write_bare>;
    auto h5_read = c2py::dispatch<h5::h5_read_bare>;
  }

} 
