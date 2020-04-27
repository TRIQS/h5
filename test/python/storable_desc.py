# Generated automatically using the command :
# c++2py storable.hpp -m storable -o storable --cxxflags="-std=c++17" --includes=./../../../c++ --includes=./../../../python --target_file_only
from cpp2py.wrap_generator import *

# The module
module = module_(full_name = "storable", doc = r"")

# Imports
module.add_imports(*['_h5py'])

# Add here all includes
module.add_include("storable.hpp")

# Add here anything to add in the C++ code at the start, e.g. namespace using
module.add_preamble("""
#include <cpp2py/converters/string.hpp>
#include <cpp2py/converters/vector.hpp>

""")


# The class storable
c = class_(
        py_type = "Storable",  # name of the python class
        c_type = "storable",   # name of the C++ class
        doc = r"""""",   # doc of the C++ class
        hdf5 = True,
)

c.add_member(c_name = "vec",
             c_type = "std::vector<int>",
             read_only= False,
             doc = r"""""")

c.add_member(c_name = "s",
             c_type = "std::string",
             read_only= False,
             doc = r"""""")

c.add_constructor("""()""", doc = r"""""")

c.add_method("""std::string hdf5_format ()""",
             is_static = True,
             doc = r"""""")

module.add_class(c)



module.generate_code()
