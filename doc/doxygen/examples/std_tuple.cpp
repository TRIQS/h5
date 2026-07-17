#include <h5/h5.hpp>
#include <iostream>
#include <tuple>

int main() {
  // create file in read/write mode
  h5::file file("tuple.h5", 'w');

  // write tuple
  h5::write(file, "mytuple", std::tuple { 3.1415f, 291827ul, 'a' });

  // read into tuple
  std::tuple<float, unsigned long, char> tup;
  h5::read(file, "mytuple", tup);

  // output tuple
  std::apply([](auto f, auto u, auto c) {
    std::cout << "(" << f << ", " << u << ", " << c << ")" << std::endl;
  }, tup);
}
