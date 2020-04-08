# Generated automatically using the command :
# c++2py h5py_io.hpp --members_read_only -N h5 -a _h5py -m _h5py -o _h5py --moduledoc="A lightweight hdf5 python interface" --cxxflags="-std=c++17" --includes=./../../c++ --only="h5_object file group h5_read_bare h5_write_bare"
from cpp2py.wrap_generator import *

# The module
module = module_(full_name = "_h5py", doc = r"A lightweight hdf5 python interface", app_name = "_h5py")

# Imports

# Add here all includes
module.add_include("<h5py_io.hpp>")

# Add here anything to add in the C++ code at the start, e.g. namespace using
module.add_preamble("""
#include <cpp2py/converters/string.hpp>
#include <cpp2py/converters/vector.hpp>

using namespace h5;
""")


# The class file
c = class_(
        py_type = "File",  # name of the python class
        c_type = "file",   # name of the C++ class
        doc = r"""A little handler for the HDF5 file

  The class is basically a pointer to the file.""",   # doc of the C++ class
        hdf5 = False,
)

c.add_constructor("""(std::string name, char mode)""", doc = r"""""")

c.add_property(name = "name", getter = cfunction("""std::string name ()"""),
             doc = r"""Name of the file""")

c.add_method("""void flush ()""",
             doc = r"""Flush the file""")

module.add_class(c)


# The class group
c = class_(
        py_type = "Group",  # name of the python class
        c_type = "group",   # name of the C++ class
        doc = r"""HDF5 group""",   # doc of the C++ class
        hdf5 = False,
)

c.add_constructor("""(file f)""", doc = r"""Takes the "/" group at the top of the file""")

c.add_property(name = "name", getter = cfunction("""std::string name ()"""),
             doc = r"""Name of the group""")

c.add_method("""group open_group (std::string key)""",
             doc = r"""Open a subgroup.
 Throws std::runtime_error if it does not exist.

Parameters
----------
key
     The name of the subgroup. If empty, return this group""")

c.add_method("""group create_group (std::string key, bool delete_if_exists = true)""",
             doc = r"""Create a subgroup in this group

Parameters
----------
key
     The name of the subgroup. If empty, return this group.

delete_if_exists
     Unlink the group if it exists""")

c.add_method("""std::vector<std::string> get_all_subgroup_dataset_names ()""", name='keys',
             doc = r"""Returns all names of dataset of G""")

c.add_property(name = "file", getter = cfunction("""file get_file ()"""),
             doc = r"""The parent file""")

c.add_method("""bool has_subgroup (std::string key)""",
             doc = r"""True iff key is a subgroup of this.

Parameters
----------
key""")

c.add_method("""bool has_dataset (std::string key)""",
             doc = r"""True iff key is a dataset of this.

Parameters
----------
key""")

c.add_method("void write_attribute(std::string key, std::string val)", calling_pattern = "h5_write_attribute(self_c, key, val)", doc = "Write an attribute")

c.add_method("std::string read_attribute(std::string name)", calling_pattern = "std::string result = h5_read_attribute<std::string>(self_c, name)", doc = "Read an attribute")

c.add_method("std::string read_hdf5_format_from_key(std::string key)", calling_pattern = "std::string result; read_hdf5_format_from_key(self_c, key, result);", doc = "Read the format string from the key in the group")

module.add_class(c)

module.add_function (name = "h5_write", signature = "void h5_write_bare (group g, std::string name, PyObject * ob)", doc = r"""""")

module.add_function (name = "h5_read", signature = "PyObject * h5_read_bare (group g, std::string name)", doc = r"""""")



module.generate_code()
