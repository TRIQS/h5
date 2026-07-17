#include <h5/h5.hpp>
#include <iostream>
#include <variant>
#include <string>

int main() {
  // create file in read/write mode
  h5::file file("variant.h5", 'w');

  // write variant
  h5::write(file, "myvariant", std::variant<double, std::string> { "Hello, World!" });

  // read into variant
  std::variant<double, std::string> var;
  h5::read(file, "myvariant", var);

  // output variant
  std::visit([](const auto& x) { std::cout << x << std::endl; }, var);
}
