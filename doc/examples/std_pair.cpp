#include <h5/h5.hpp>
#include <iostream>
#include <utility>

int main() {
  // create file in read/write mode
  h5::file file("pair.h5", 'w');

  // write pair
  h5::write(file, "mypair", std::pair { 3.1415f, 291827ul });

  // read into pair
  std::pair<float, unsigned long> p;
  h5::read(file, "mypair", p);

  // output pair
  std::cout << "(" << p.first << ", " << p.second << ")" << std::endl;
}
