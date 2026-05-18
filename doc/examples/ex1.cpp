#include <h5/h5.hpp>
#include <iostream>
#include <numeric>
#include <vector>

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

  // read data from file
  h5::array_interface::read(file, "view", read_view, read_slab);

  // output data
  for (int i = 0; i < rows_r; ++i) {
    for (int j = 0; j < cols_r; ++j) {
      std::cout << read_data[i * cols_r + j] << " ";
    }
    std::cout << std::endl;
  }
}
