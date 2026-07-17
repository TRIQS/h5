@page ex1 Example 1: Write/Read a 2-dimensional array

[TOC]

This example shows how to use the **h5** array interface to write/read a 2-dimensional array to/from HDF5.

We first write a 2-dimensional `5x5` array to an HDF5 file.

Then we use an h5::array_interface::hyperslab to select every other column from the original `5x5` matrix and
read it into a `5x3` h5::array_interface::array_view.

Finally, we output the result to stdout.

@include ex1.cpp

Output:

```
0 2 4
5 7 9
10 12 14
15 17 19
20 22 24
```

Contents of `view.h5`:

```
HDF5 "view.h5" {
GROUP "/" {
   DATASET "view" {
      DATATYPE  H5T_STD_I32LE
      DATASPACE  SIMPLE { ( 5, 5 ) / ( 5, 5 ) }
      DATA {
      (0,0): 0, 1, 2, 3, 4,
      (1,0): 5, 6, 7, 8, 9,
      (2,0): 10, 11, 12, 13, 14,
      (3,0): 15, 16, 17, 18, 19,
      (4,0): 20, 21, 22, 23, 24
      }
   }
}
}
```
