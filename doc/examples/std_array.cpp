#include <h5/h5.hpp>
#include <iostream>
#include <array>

int main() {
  // create file in read/write mode
  h5::file file("array.h5", 'w');

  // write array
  h5::write(file, "myarray", std::array{1, 2, 3, 4, 5});

  // read into array
  std::array<int, 5> arr{};
  h5::read(file, "myarray", arr);

  // output array
  for (auto x : arr) std::cout << x << std::endl;
}
