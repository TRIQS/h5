#include <h5/h5.hpp>
#include <iostream>
#include <map>
#include <string>

int main() {
  // create file in read/write mode
  h5::file file("map.h5", 'w');

  // write map
  h5::write(file, "mymap", std::map<std::string, int> { { "a", 1 }, { "b", 2 }, { "c", 3 } });

  // read into map
  std::map<std::string, int> map;
  h5::read(file, "mymap", map);

  // output map
  for (auto [x, y] : map) std::cout << "(" << x << ", " << y << ")" << std::endl;
}
