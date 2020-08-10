[![build](https://github.com/TRIQS/h5/workflows/build/badge.svg?branch=unstable)](https://github.com/TRIQS/h5/actions?query=workflow%3Abuild)

# h5

h5 is a high-level C++ interface to the [hdf5 library](https://www.hdfgroup.org/solutions/hdf5/), which
* Provides write and read functions for various [STL container types](https://en.cppreference.com/w/cpp/container).
* Provides a generic `array_interface` to store and load multi-dimensional arrays
* Takes an [RAII](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization) approach to manage the reference counting.
* Comes with Python bindings.

## Simple Example

Storing and loading a vector of string and a vector of doubles

```c++
#include <h5/h5.hpp>
#include <string>
#include <vector>

int main(){

  {
    // Open file in write mode
    h5::file file("vec.h5", 'w');

    std::vector<std::string> vecs = {"a", "b"};
    std::vector<double> vecd      = {1.0, 2.0};

    h5::write(file, "vecs", vecs);
    h5::write(file, "vecd", vecd);

  } // Close file

  {
    // Open file in read mode
    h5::file file("vec.h5", 'r');

    std::vector<std::string> vecs;
    std::vector<double> vecd;

    h5::read(file, "vecs", vecs);
    h5::read(file, "vecd", vecd);

  } // Close file
}
```

This example will generate an hdf5 file `vec.h5` with two datasets

```
$ h5dump vec.h5

HDF5 "vec.h5" {
GROUP "/" {
   DATASET "vecd" {
      DATATYPE  H5T_IEEE_F64LE
      DATASPACE  SIMPLE { ( 2 ) / ( 2 ) }
      DATA {
      (0): 1, 2
      }
   }
   DATASET "vecs" {
      DATATYPE  H5T_STRING {
         STRSIZE 2;
         STRPAD H5T_STR_NULLTERM;
         CSET H5T_CSET_UTF8;
         CTYPE H5T_C_S1;
      }
      DATASPACE  SIMPLE { ( 2 ) / ( 2 ) }
      DATA {
      (0): "a", "b"
      }
   }
}
}
```

For further examples we refer the users to our [tests](https://github.com/TRIQS/h5/tree/unstable/test/c++).
