#include <h5/h5.hpp>
#include <iostream>
#include <string>

int main() {
  // create file in read/write mode
  h5::file file("string.h5", 'w');

  // write string
  h5::write(file, "mystring", "Hello world!");

  // read into string
  std::string str;
  h5::read(file, "mystring", str);

  // output string
  std::cout << str << std::endl;
}
