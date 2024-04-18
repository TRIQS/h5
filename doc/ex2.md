@page ex2 Example 2: Write/Read a custom C++ type

[TOC]

This example shows how to make your own C++ type HDF5 serializable.

We first define a class `foo` that we want to write/read to/from HDF5.

The most important functions are `h5_write` and `h5_read`. They are called by the generic h5::write and h5::read
implementations using argument-dependent lookup (ADL).

The `hdf5_format` function is optional and can be used to attach a string attribute to `foo` objects.
When reading from HDF5, it allows us to verify that data that we are reading is actually of type `foo`.

In `main()`, we simply create an HDF5 file, write an object of type `foo`, read the same object from
the file and output the result to stdout.

```cpp
#include <h5/h5.hpp>
#include <iostream>
#include <string>

// define HDF5 serializable type
class foo {
private:
  int i;
  double d;
  std::string s;

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
    h5::write_hdf5_format(sg, f);
    h5::write(sg, "i", f.i);
    h5::write(sg, "d", f.d);
    h5::write(sg, "s", f.s);
  }

  // read from HDF5
  friend void h5_read(h5::group g, const std::string& subgroup_name, foo& f) {
    auto sg = g.open_group(subgroup_name);
    h5::assert_hdf5_format(sg, f);
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
```

Output:

```
1 2.2 three
```

Contents of `foo.h5`:

```
HDF5 "foo.h5" {
GROUP "/" {
   GROUP "myfoo" {
      ATTRIBUTE "Format" {
         DATATYPE  H5T_STRING {
            STRSIZE H5T_VARIABLE;
            STRPAD H5T_STR_NULLTERM;
            CSET H5T_CSET_UTF8;
            CTYPE H5T_C_S1;
         }
         DATASPACE  SCALAR
         DATA {
         (0): "foo"
         }
      }
      DATASET "d" {
         DATATYPE  H5T_IEEE_F64LE
         DATASPACE  SCALAR
         DATA {
         (0): 2.2
         }
      }
      DATASET "i" {
         DATATYPE  H5T_STD_I32LE
         DATASPACE  SCALAR
         DATA {
         (0): 1
         }
      }
      DATASET "s" {
         DATATYPE  H5T_STRING {
            STRSIZE H5T_VARIABLE;
            STRPAD H5T_STR_NULLTERM;
            CSET H5T_CSET_UTF8;
            CTYPE H5T_C_S1;
         }
         DATASPACE  SCALAR
         DATA {
         (0): "three"
         }
      }
   }
}
}
```
