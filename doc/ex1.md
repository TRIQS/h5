@page ex1 Example 1: Write/Read a 2-dimensional array

[TOC]

This example shows how to use the **h5** array interface to write/read a 2-dimensional array to/from HDF5.

We first write a 2-dimensional `5x5` array to an HDF5 file.

Then we use an h5::array_interface::hyperslab to select every other column from the original `5x5` matrix and
read it into a `5x3` h5::array_interface::array_view.

Finally, we output the result to stdout.

```cpp
#include <h5/h5.hpp>
#include <iostream>
#include <numeric>
#include <vector>

// this is only needed to avoid issues with doxygen
#include <H5Tpublic.h>

int main() {
  // data to be written
  std::vector<int> data (25, 0);
  std::iota(data.begin(), data.end(), 0);

  // create an array_view of rank 2 with dimensions 5x5 of the original data
  int rank = 2;
  int rows_w = 5;
  int cols_w = 5;
  h5::array_interface::array_view view(h5::hdf5_type<int>(), (void*) data.data(), rank, false);
  view.slab.count[0] = rows_w;
  view.slab.count[1] = cols_w;
  view.parent_shape[0] = rows_w;
  view.parent_shape[1] = cols_w;

  // create file in read/write mode
  h5::file file("view.h5", 'w');

  // write array_view to file
  h5::array_interface::write(file, "view", view, false);

  // reserve memory for reading
  std::vector<int> read_data(15, 0);

  // create an array_view or rank 2 with dimensions 5x3 of the read memory
  int rows_r = 5;
  int cols_r = 3;
  h5::array_interface::array_view read_view(h5::hdf5_type<int>(), (void*) read_data.data(), rank, false);
  read_view.slab.count[0] = rows_r;
  read_view.slab.count[1] = cols_r;
  read_view.parent_shape[0] = rows_r;
  read_view.parent_shape[1] = cols_r;

  // create an hyperslab to select the data to be read from the file (every other column -> stride in second dimension is 2)
  h5::array_interface::hyperslab read_slab(rank, false);
  read_slab.count[0] = rows_r;
  read_slab.count[1] = cols_r;
  read_slab.stride[0] = 1;
  read_slab.stride[1] = 2;

  // get dataset_info from the dataset in the file
  auto ds_info = h5::array_interface::get_dataset_info(file, "view");

  // read data from file
  h5::array_interface::read(file, "view", read_view, ds_info, read_slab);

  // output data
  for (int i = 0; i < rows_r; ++i) {
    for (int j = 0; j < cols_r; ++j) {
      std::cout << read_data[i * cols_r + j] << " ";
    }
    std::cout << std::endl;
  }
}
```

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
