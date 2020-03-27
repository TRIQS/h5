#include <h5/h5.hpp>
#include <string>
#include <vector>

class storable{
  public:
  // Some Data members
  std::vector<int> vec = { 4, 2 };
  std::string s = "😎";

  // Default constructor has to be specified for cpp2py to pick it up
  storable() = default;

  // Our format string
  static std::string hdf5_format(){ return "Storable"; }

  friend void h5_write(h5::group g, std::string const & name, storable const & obj){
    auto gr = g.create_group(name); 
    h5_write_attribute(gr, "TRIQS_HDF5_data_scheme", storable::hdf5_format());
    h5_write(gr, "vec", obj.vec);
    h5_write(gr, "s", obj.s);
  }

  friend storable h5_read(h5::group g, std::string const & name, storable & obj){
    auto gr = g.open_group(name); 
    h5_read(gr, "vec", obj.vec);
    h5_read(gr, "s", obj.s);
  }
};
