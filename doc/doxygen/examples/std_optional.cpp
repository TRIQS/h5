#include <h5/h5.hpp>
#include <iostream>
#include <optional>

int main() {
  // create file in read/write mode
  h5::file file("optional.h5", 'w');

  // write optional
  h5::write(file, "myoptional", std::optional<float> { 3.1415f });

  // read into optional
  std::optional<float> val;
  h5::read(file, "myoptional", val);

  // output optional value
  std::cout << *val << std::endl;
}
