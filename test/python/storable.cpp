#include <c2py/c2py.hpp>
#include <h5/h5.hpp>

#include <string>
#include <vector>

class storable {
  public:
  // Some Data members
  std::vector<int> vec = {4, 2};
  std::string s        = "😎";

  // Default constructor has to be specified for c2py/clair to pick it up
  storable() = default;

  // Our format string
  static std::string hdf5_format() { return "Storable"; }

  friend void h5_write(h5::group g, std::string const &name, storable const &obj) {
    auto gr = g.create_group(name);
    h5::write_hdf5_format(gr, obj); // NOLINT
    h5::write(gr, "vec", obj.vec);
    h5::write(gr, "s", obj.s);
  }

  friend void h5_read(h5::group g, std::string const &name, storable &obj) {
    auto gr = g.open_group(name);
    h5::assert_hdf5_format(gr, obj); // NOLINT
    h5::read(gr, "vec", obj.vec);
    h5::read(gr, "s", obj.s);
  }
};

static_assert(h5::Storable<storable>);
