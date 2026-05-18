#include <h5/h5.hpp>
#include <iostream>
#include <string>

// define HDF5 serializable type
class foo {
private:
  int i{};
  double d{};
  std::string s{};

public:
  // constructors
  foo() = default;
  foo(int i, double d, std::string s) : i(i), d(d), s(s) {}

  // print members to stdout
  void print() const { std::cout << i << " " << d << " " << s << std::endl; }

  // get hdf5_format tag
  static std::string hdf5_format() { return "foo";}

  // write to HDF5
  friend void h5_write(h5::group g, const std::string& subgroup_name, const foo& f) {
    auto sg = g.create_group(subgroup_name);
    h5::write_hdf5_format(sg, f); // NOLINT (cppcoreguidelines-slicing)
    h5::write(sg, "i", f.i);
    h5::write(sg, "d", f.d);
    h5::write(sg, "s", f.s);
  }

  // read from HDF5
  friend void h5_read(h5::group g, const std::string& subgroup_name, foo& f) {
    auto sg = g.open_group(subgroup_name);
    h5::assert_hdf5_format(sg, f); // NOLINT (cppcoreguidelines-slicing)
    h5::read(sg, "i", f.i);
    h5::read(sg, "d", f.d);
    h5::read(sg, "s", f.s);
  }
};

int main() {
  // create file in read/write mode
  h5::file file("foo.h5", 'w');

  // write foo
  h5::write(file, "myfoo", foo(1, 2.2, "three"));

  // read foo
  foo f;
  h5::read(file, "myfoo", f);

  // output foo
  f.print();
}
