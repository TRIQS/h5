@page ex2 Example 2: Write/Read a custom C++ type

[TOC]

This example shows how to make your own C++ type HDF5 serializable.

We first define a class `foo` that we want to write/read to/from HDF5.

The most important functions are `h5_write` and `h5_read`. They are called by the generic h5::write and h5::read
implementations using argument-dependent lookup (ADL).

It is recommended to also implement the `hdf5_format` function which can be used to attach a string attribute to `foo` 
objects.
When reading from HDF5, it allows us to verify that data that we are reading is actually of type `foo` using
h5::assert_hdf5_format.

In `main()`, we simply create an HDF5 file, write an object of type `foo`, read the same object from
the file and output the result to stdout.

@include ex2.cpp

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
