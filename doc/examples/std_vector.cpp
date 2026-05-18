#include <h5/h5.hpp>
#include <iostream>
#include <vector>

int main() {
  // create file in read/write mode
  h5::file file("vec.h5", 'w');

  // write vector
  h5::write(file, "myvec", std::vector<double> { 1.1, 2.2, 3.3 });

  // read into vector
  std::vector<double> vec;
  h5::read(file, "myvec", vec);

  // output vector
  for (auto x : vec) std::cout << x << std::endl;
}
